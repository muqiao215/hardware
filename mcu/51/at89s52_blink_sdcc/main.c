#include <8052.h>

// 假设 8 个 LED 接在 P1 口
// 低电平(0)点亮，高电平(1)熄灭

void delay(unsigned int time) {
    volatile unsigned int i, j;
    for (i = 0; i < time; i++) {
        for (j = 0; j < 120; j++);
    }
}

void main() {
    // 初始状态：P1.0 亮 (二进制 1111 1110 = 0xFE)
    unsigned char pattern = 0xFE;

    while (1) {
        P1 = pattern;
        delay(200); // 调节延时可改变流速

        // 循环左移算法 (8位)
        // 将最高位移到最低位，实现顺时针流水
        // 例如: 1111 1110 -> 1111 1101
        pattern = (pattern << 1) | (pattern >> 7);
    }
}
