#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

// 蜂鸣器控制
#define BUZZER_ON()   GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define BUZZER_OFF()  GPIO_SetBits(GPIOB, GPIO_Pin_12)

void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    BUZZER_OFF();
}

void Buzzer_Beep(uint16_t ms)
{
    BUZZER_ON();
    Delay_ms(ms);
    BUZZER_OFF();
}

// 开机启动动画 - 逐行显示
void Boot_Animation(void)
{
    OLED_Clear();

    // 第一行：系统名称（打字机效果）
    char *title = "STM32 System";
    for (int i = 0; title[i] != '\0'; i++)
    {
        OLED_ShowChar(1, 3 + i, title[i]);
        Delay_ms(50);
    }
    Buzzer_Beep(50);

    // 第二行：版本号
    Delay_ms(200);
    OLED_ShowString(2, 1, "================");
    Buzzer_Beep(30);

    // 第三行：加载进度条
    OLED_ShowString(3, 1, "Loading:");
    for (int i = 0; i < 8; i++)
    {
        OLED_ShowChar(3, 9 + i, '#');
        Delay_ms(100);
    }
    Buzzer_Beep(50);

    // 第四行：完成
    OLED_ShowString(4, 4, "[  READY  ]");
    Buzzer_Beep(100);

    Delay_ms(1000);
}

// 主界面显示
void Show_MainScreen(uint32_t counter, uint32_t uptime_sec)
{
    // 第一行：标题
    OLED_ShowString(1, 1, "=== MUQIAO ===");

    // 第二行：计数器
    OLED_ShowString(2, 1, "Count:");
    OLED_ShowNum(2, 8, counter, 6);

    // 第三行：运行时间
    OLED_ShowString(3, 1, "Time:");
    uint8_t min = uptime_sec / 60;
    uint8_t sec = uptime_sec % 60;
    OLED_ShowNum(3, 7, min, 2);
    OLED_ShowChar(3, 9, ':');
    OLED_ShowNum(3, 10, sec, 2);
    OLED_ShowString(3, 13, "s");

    // 第四行：状态指示（动态闪烁）
    static uint8_t blink = 0;
    blink = !blink;
    if (blink)
    {
        OLED_ShowString(4, 1, "Status: RUNNING");
    }
    else
    {
        OLED_ShowString(4, 1, "Status:        ");
    }
}

// 显示十六进制计数（炫酷效果）
void Show_HexCounter(uint32_t value)
{
    OLED_ShowString(2, 1, "HEX: 0x");
    OLED_ShowHexNum(2, 8, value, 8);
}

int main(void)
{
    // 初始化
    Buzzer_Init();
    OLED_Init();

    // 播放开机动画
    Boot_Animation();

    // 清屏准备主界面
    OLED_Clear();

    uint32_t counter = 0;
    uint32_t uptime = 0;
    uint8_t tick = 0;

    while (1)
    {
        // 更新主界面
        Show_MainScreen(counter, uptime);

        // 十六进制显示
        Show_HexCounter(counter);

        // 计数器递增
        counter++;

        // 每10次循环增加1秒（约500ms一次循环）
        tick++;
        if (tick >= 2)
        {
            tick = 0;
            uptime++;

            // 每10秒蜂鸣一次
            if (uptime % 10 == 0)
            {
                Buzzer_Beep(20);
            }
        }

        Delay_ms(500);
    }
}
