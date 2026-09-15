#include "delay.h"
#include "tim2_pwm.h"

int main(void) {
    uint16_t duty = 0;
    int8_t step = 25;

    tim2_pwm_init(1000, 72);

    while (1) {
        tim2_pwm_set_duty(duty);
        delay_ms(20);
        duty = (uint16_t)(duty + step);
        if (duty >= 1000 || duty == 0) {
            step = (int8_t)-step;
        }
    }
}
