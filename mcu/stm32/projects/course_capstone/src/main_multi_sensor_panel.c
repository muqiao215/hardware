#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "Delay.h"
#include "MPU6050.h"
#include "OLED.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BUZZER_GPIO GPIOA
#define BUZZER_PIN GPIO_Pin_0
#define BUTTON_GPIO GPIOA
#define BUTTON_PIN GPIO_Pin_1
#define DHT11_GPIO GPIOA
#define DHT11_PIN GPIO_Pin_4

#define ADC_LIGHT_INDEX 0
#define ADC_TCRT_INDEX 1

#define LIGHT_DARK_THRESHOLD 1800u
#define TCRT_REFLECT_THRESHOLD 2200u
#define TEMP_WARN_C 35u
#define HUMI_WARN_PERCENT 85u

static float rad2deg(float r)
{
    return r * 57.2957795f;
}

typedef enum
{
    BUTTON_NONE = 0,
    BUTTON_SHORT,
    BUTTON_LONG,
} ButtonEvent;

static void show_line(uint8_t line, const char *fmt, ...)
{
    char text[17];
    char tmp[40];
    va_list args;

    memset(text, ' ', 16);
    text[16] = '\0';

    va_start(args, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);

    for (uint8_t i = 0; i < 16 && tmp[i] != '\0'; i++)
    {
        text[i] = tmp[i];
    }

    OLED_ShowString(line, 1, text);
}

static uint8_t percent_of_adc(uint16_t value)
{
    return (uint8_t)(((uint32_t)value * 100u) / 4095u);
}

static void io_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);
}

static void buzzer_on(void)
{
    GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);
}

static void buzzer_off(void)
{
    GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN);
}

static void dht11_pin_output(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO, &GPIO_InitStructure);
}

static void dht11_pin_input(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO, &GPIO_InitStructure);
}

static uint8_t dht11_wait_level(uint8_t level, uint16_t timeout_us)
{
    while (timeout_us--)
    {
        if (GPIO_ReadInputDataBit(DHT11_GPIO, DHT11_PIN) == level)
        {
            return 1;
        }
        Delay_us(1);
    }

    return 0;
}

static uint8_t dht11_read(uint8_t *temperature, uint8_t *humidity)
{
    uint8_t data[5] = {0};

    dht11_pin_output();
    GPIO_ResetBits(DHT11_GPIO, DHT11_PIN);
    Delay_ms(20);
    GPIO_SetBits(DHT11_GPIO, DHT11_PIN);
    Delay_us(30);
    dht11_pin_input();

    if (!dht11_wait_level(0, 100))
        return 0;
    if (!dht11_wait_level(1, 100))
        return 0;
    if (!dht11_wait_level(0, 100))
        return 0;

    for (uint8_t byte = 0; byte < 5; byte++)
    {
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (!dht11_wait_level(1, 70))
                return 0;

            Delay_us(40);
            if (GPIO_ReadInputDataBit(DHT11_GPIO, DHT11_PIN) != 0)
            {
                data[byte] |= (uint8_t)(0x80u >> bit);
                if (!dht11_wait_level(0, 80))
                    return 0;
            }
        }
    }

    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        return 0;
    }

    *humidity = data[0];
    *temperature = data[2];
    return 1;
}

static ButtonEvent read_button_event(void)
{
    uint16_t held_ms = 0;

    if (GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN) != 0)
    {
        return BUTTON_NONE;
    }

    Delay_ms(20);
    if (GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN) != 0)
    {
        return BUTTON_NONE;
    }

    while (GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN) == 0)
    {
        Delay_ms(20);
        if (held_ms < 2000)
        {
            held_ms += 20;
        }
    }
    Delay_ms(20);

    if (held_ms >= 800)
    {
        return BUTTON_LONG;
    }

    return BUTTON_SHORT;
}

static void show_summary(uint16_t light, uint16_t tcrt, uint8_t temp, uint8_t humi, uint8_t dht_ok, uint8_t muted)
{
    uint8_t dark = light < LIGHT_DARK_THRESHOLD;
    uint8_t object = tcrt > TCRT_REFLECT_THRESHOLD;
    uint8_t dht_warn = dht_ok && ((temp >= TEMP_WARN_C) || (humi >= HUMI_WARN_PERCENT));

    show_line(1, "Multi Sensor %s", muted ? "M" : " ");
    show_line(2, "L:%4u %s", (unsigned)light, dark ? "DARK" : "OK");
    show_line(3, "R:%4u %s", (unsigned)tcrt, object ? "HIT" : "----");
    if (dht_ok)
    {
        show_line(4, "T:%2uC H:%2u%% %s", (unsigned)temp, (unsigned)humi, dht_warn ? "!" : "OK");
    }
    else
    {
        show_line(4, "DHT11 ERR PA4");
    }
}

static void show_raw(uint16_t light, uint16_t tcrt, uint8_t temp, uint8_t humi, uint8_t dht_ok, uint8_t page)
{
    show_line(1, "ADC Raw Page %u", (unsigned)(page + 1));
    show_line(2, "PA2 L:%4u %3u%%", (unsigned)light, (unsigned)percent_of_adc(light));
    show_line(3, "PA3 R:%4u %3u%%", (unsigned)tcrt, (unsigned)percent_of_adc(tcrt));
    if (dht_ok)
    {
        show_line(4, "PA4 T:%2u H:%2u", (unsigned)temp, (unsigned)humi);
    }
    else
    {
        show_line(4, "PA4 DHT no data");
    }
}

static void show_thresholds(uint8_t muted)
{
    show_line(1, "Thresholds %s", muted ? "MUTE" : "BEEP");
    show_line(2, "Dark  < %4u", (unsigned)LIGHT_DARK_THRESHOLD);
    show_line(3, "Refl  > %4u", (unsigned)TCRT_REFLECT_THRESHOLD);
    show_line(4, "T>=%2u H>=%2u", (unsigned)TEMP_WARN_C, (unsigned)HUMI_WARN_PERCENT);
}

static void show_mpu_accel(uint8_t mpu_id, int16_t ax, int16_t ay, int16_t az)
{
    if (mpu_id == 0x68)
    {
        show_line(1, "MPU Acc ID:68");
        show_line(2, "AX:%7d", (int)ax);
        show_line(3, "AY:%7d", (int)ay);
        show_line(4, "AZ:%7d", (int)az);
    }
    else
    {
        show_line(1, "MPU6050 ERR");
        show_line(2, "Need PB10 SCL");
        show_line(3, "Need PB11 SDA");
        show_line(4, "ID:0x%02X", (unsigned)mpu_id);
    }
}

static void show_mpu_gyro(uint8_t mpu_id, int16_t gx, int16_t gy, int16_t gz)
{
    if (mpu_id == 0x68)
    {
        show_line(1, "MPU Gyro ID:68");
        show_line(2, "GX:%7d", (int)gx);
        show_line(3, "GY:%7d", (int)gy);
        show_line(4, "GZ:%7d", (int)gz);
    }
    else
    {
        show_line(1, "MPU6050 ERR");
        show_line(2, "Need PB10 SCL");
        show_line(3, "Need PB11 SDA");
        show_line(4, "ID:0x%02X", (unsigned)mpu_id);
    }
}

static void show_mpu_angle(uint8_t mpu_id, float roll, float pitch)
{
    if (mpu_id == 0x68)
    {
        show_line(1, "MPU Angle");
        show_line(2, "Roll :%7.1f", roll);
        show_line(3, "Pitch:%7.1f", pitch);
        show_line(4, "Short key next");
    }
    else
    {
        show_line(1, "MPU6050 ERR");
        show_line(2, "Need PB10 SCL");
        show_line(3, "Need PB11 SDA");
        show_line(4, "ID:0x%02X", (unsigned)mpu_id);
    }
}

int main(void)
{
    const uint8_t adc_channels[2] = {2, 3};
    uint8_t page = 0;
    uint8_t muted = 0;
    uint16_t beep_tick = 0;
    uint16_t dht_poll_tick = 0;
    uint8_t dht_ok = 0;
    uint8_t temp = 0;
    uint8_t humi = 0;
    uint8_t mpu_id = 0;
    float roll = 0.0f;
    float pitch = 0.0f;
    int16_t ax = 0;
    int16_t ay = 0;
    int16_t az = 0;
    int16_t gx = 0;
    int16_t gy = 0;
    int16_t gz = 0;

    io_init();
    OLED_Init();
    dht11_pin_input();
    ADC_DMA_Init(adc_channels, 2);
    MPU6050_Init();
    mpu_id = MPU6050_GetID();

    OLED_Clear();
    show_line(1, "Multi Sensor");
    show_line(2, "PA2/PA3 ADC");
    show_line(3, "PA4 DHT11 DATA");
    show_line(4, "PA1 page/mute");
    Delay_ms(1200);
    OLED_Clear();
    show_line(3, "PA1 page/mute");
    show_line(4, "Hold PA1 mute");
    Delay_ms(600);

    while (1)
    {
        const volatile uint16_t *adc = ADC_DMA_GetValues();
        uint16_t light = adc[ADC_LIGHT_INDEX];
        uint16_t tcrt = adc[ADC_TCRT_INDEX];

        if (dht_poll_tick == 0)
        {
            dht_ok = dht11_read(&temp, &humi);
            mpu_id = MPU6050_GetID();
        }
        dht_poll_tick = (uint16_t)((dht_poll_tick + 1) % 25);

        if (mpu_id == 0x68)
        {
            MPU6050_GetData(&ax, &ay, &az, &gx, &gy, &gz);
            float fax = (float)ax / 2048.0f;
            float fay = (float)ay / 2048.0f;
            float faz = (float)az / 2048.0f;
            roll = rad2deg(atan2f(fay, faz));
            pitch = rad2deg(atan2f(-fax, sqrtf(fay * fay + faz * faz)));
        }

        uint8_t alarm = (light < LIGHT_DARK_THRESHOLD) ||
                        (tcrt > TCRT_REFLECT_THRESHOLD) ||
                        (dht_ok && ((temp >= TEMP_WARN_C) || (humi >= HUMI_WARN_PERCENT)));

        ButtonEvent event = read_button_event();
        if (event == BUTTON_SHORT)
        {
            page = (uint8_t)((page + 1) % 6);
        }
        else if (event == BUTTON_LONG)
        {
            muted = !muted;
            buzzer_off();
        }

        if (page == 0)
        {
            show_summary(light, tcrt, temp, humi, dht_ok, muted);
        }
        else if (page == 1)
        {
            show_raw(light, tcrt, temp, humi, dht_ok, page);
        }
        else if (page == 2)
        {
            show_thresholds(muted);
        }
        else if (page == 3)
        {
            show_mpu_accel(mpu_id, ax, ay, az);
        }
        else if (page == 4)
        {
            show_mpu_gyro(mpu_id, gx, gy, gz);
        }
        else
        {
            show_mpu_angle(mpu_id, roll, pitch);
        }

        if (alarm && !muted)
        {
            if (beep_tick == 0)
            {
                buzzer_on();
            }
            else if (beep_tick == 1)
            {
                buzzer_off();
            }
            beep_tick = (uint16_t)((beep_tick + 1) % 6);
        }
        else
        {
            beep_tick = 0;
            buzzer_off();
        }

        Delay_ms(80);
    }
}
