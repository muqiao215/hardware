#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Delay.h"
#include "MPU6050.h"
#include "MyRTC.h"
#include "OLED.h"
#include "Serial.h"
#include "W25Q64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple logger: records stored sequentially in W25Q64, dumped as CSV over UART.
 * Record size is 32 bytes.
 */

#define LOG_BASE 0x001000
#define LOG_SECTOR_SIZE 4096
#define LOG_RECORD_SIZE 32
#define LOG_MAX_SECTORS 8 /* 32KB */

static uint32_t s_wr = 0;
static uint8_t s_running = 0;
static uint32_t s_seq = 0;

static void log_erase_all(void)
{
    for (uint32_t s = 0; s < LOG_MAX_SECTORS; s++)
    {
        W25Q64_SectorErase(LOG_BASE + s * LOG_SECTOR_SIZE);
    }
    s_wr = 0;
    s_seq = 0;
}

static void log_write_one(void)
{
    int16_t ax, ay, az, gx, gy, gz;
    MPU6050_GetData(&ax, &ay, &az, &gx, &gy, &gz);
    const volatile uint16_t *adc = ADC_DMA_GetValues();

    uint8_t rec[LOG_RECORD_SIZE];
    memset(rec, 0, sizeof(rec));
    rec[0] = 0xCA;
    rec[1] = 0xFE;
    rec[2] = (uint8_t)(s_seq >> 0);
    rec[3] = (uint8_t)(s_seq >> 8);
    rec[4] = (uint8_t)(s_seq >> 16);
    rec[5] = (uint8_t)(s_seq >> 24);

    uint32_t ts = RTC_GetCounter();
    rec[6] = (uint8_t)(ts >> 0);
    rec[7] = (uint8_t)(ts >> 8);
    rec[8] = (uint8_t)(ts >> 16);
    rec[9] = (uint8_t)(ts >> 24);

    int16_t vals[6] = {ax, ay, az, gx, gy, gz};
    for (int i = 0; i < 6; i++)
    {
        rec[10 + i * 2] = (uint8_t)(vals[i] >> 0);
        rec[11 + i * 2] = (uint8_t)(vals[i] >> 8);
    }

    for (int i = 0; i < 4; i++)
    {
        rec[22 + i * 2] = (uint8_t)(adc[i] >> 0);
        rec[23 + i * 2] = (uint8_t)(adc[i] >> 8);
    }

    uint32_t addr = LOG_BASE + s_wr;
    W25Q64_PageProgram(addr, rec, LOG_RECORD_SIZE);

    s_wr += LOG_RECORD_SIZE;
    s_seq++;
    uint32_t cap = LOG_MAX_SECTORS * LOG_SECTOR_SIZE;
    if (s_wr >= cap)
    {
        s_wr = 0;
    }
}

static void log_dump(uint32_t n)
{
    Serial_SendString("seq,ts,ax,ay,az,gx,gy,gz,ad0,ad1,ad2,ad3\r\n");

    uint32_t cap = LOG_MAX_SECTORS * LOG_SECTOR_SIZE;
    uint32_t total = cap / LOG_RECORD_SIZE;
    if (n > total)
        n = total;

    uint8_t rec[LOG_RECORD_SIZE];
    for (uint32_t i = 0; i < n; i++)
    {
        uint32_t addr = LOG_BASE + i * LOG_RECORD_SIZE;
        W25Q64_ReadData(addr, rec, LOG_RECORD_SIZE);
        if (!(rec[0] == 0xCA && rec[1] == 0xFE))
            continue;

        uint32_t seq = (uint32_t)rec[2] | ((uint32_t)rec[3] << 8) | ((uint32_t)rec[4] << 16) | ((uint32_t)rec[5] << 24);
        uint32_t ts = (uint32_t)rec[6] | ((uint32_t)rec[7] << 8) | ((uint32_t)rec[8] << 16) | ((uint32_t)rec[9] << 24);
        int16_t ax = (int16_t)((uint16_t)rec[10] | ((uint16_t)rec[11] << 8));
        int16_t ay = (int16_t)((uint16_t)rec[12] | ((uint16_t)rec[13] << 8));
        int16_t az = (int16_t)((uint16_t)rec[14] | ((uint16_t)rec[15] << 8));
        int16_t gx = (int16_t)((uint16_t)rec[16] | ((uint16_t)rec[17] << 8));
        int16_t gy = (int16_t)((uint16_t)rec[18] | ((uint16_t)rec[19] << 8));
        int16_t gz = (int16_t)((uint16_t)rec[20] | ((uint16_t)rec[21] << 8));

        uint16_t ad0 = (uint16_t)rec[22] | ((uint16_t)rec[23] << 8);
        uint16_t ad1 = (uint16_t)rec[24] | ((uint16_t)rec[25] << 8);
        uint16_t ad2 = (uint16_t)rec[26] | ((uint16_t)rec[27] << 8);
        uint16_t ad3 = (uint16_t)rec[28] | ((uint16_t)rec[29] << 8);

        Serial_Printf("%lu,%lu,%d,%d,%d,%d,%d,%d,%u,%u,%u,%u\r\n",
                      (unsigned long)seq,
                      (unsigned long)ts,
                      (int)ax,
                      (int)ay,
                      (int)az,
                      (int)gx,
                      (int)gy,
                      (int)gz,
                      (unsigned)ad0,
                      (unsigned)ad1,
                      (unsigned)ad2,
                      (unsigned)ad3);
    }
}

static void cmd(char *line)
{
    if (strncmp(line, "start", 5) == 0)
    {
        s_running = 1;
        Serial_SendString("OK start\r\n");
        return;
    }
    if (strncmp(line, "stop", 4) == 0)
    {
        s_running = 0;
        Serial_SendString("OK stop\r\n");
        return;
    }
    if (strncmp(line, "clear", 5) == 0)
    {
        log_erase_all();
        Serial_SendString("OK clear\r\n");
        return;
    }
    if (strncmp(line, "dump", 4) == 0)
    {
        uint32_t n = (uint32_t)atoi(line + 4);
        if (n == 0)
            n = 64;
        log_dump(n);
        return;
    }
    if (strncmp(line, "status", 6) == 0)
    {
        Serial_Printf("running=%u seq=%lu wr=0x%lx\r\n", (unsigned)s_running, (unsigned long)s_seq, (unsigned long)s_wr);
        return;
    }

    Serial_SendString("cmd: start|stop|clear|dump <n>|status\r\n");
}

int main(void)
{
    OLED_Init();
    Serial_Init(9600);
    MyRTC_Init();
    MPU6050_Init();
    W25Q64_Init();

    uint8_t ch[4] = {0, 1, 2, 3};
    ADC_DMA_Init(ch, 4);

    OLED_Clear();
    OLED_ShowString(1, 1, "Data Logger");
    OLED_ShowString(2, 1, "UART:9600 CSV");
    OLED_ShowString(3, 1, "cmd:start/stop");
    OLED_ShowString(4, 1, "dump 128 / clear");

    Serial_SendString("\r\n[data_logger] ready\r\n");
    Serial_SendString("cmd: start|stop|clear|dump <n>|status\r\n");

    uint32_t tick = 0;
    while (1)
    {
        char line[128];
        if (Serial_ReadLine(line, sizeof(line)))
        {
            cmd(line);
        }

        if (s_running)
        {
            if ((tick++ % 20) == 0)
            {
                log_write_one(); /* 200ms */
            }
        }

        Delay_ms(10);
    }
}

