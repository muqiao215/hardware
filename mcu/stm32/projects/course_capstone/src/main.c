#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

int main(void)
{
    OLED_Init();
    OLED_ShowString(1, 1, "Course Capstone");
    OLED_ShowString(2, 1, "Use -DAPP=xxx");
    OLED_ShowString(3, 1, "APP:");
    OLED_ShowString(3, 6, CAPSTONE_APP_NAME);
    OLED_ShowString(4, 1, "Build & flash");

    while (1)
    {
        Delay_ms(500);
    }
}
