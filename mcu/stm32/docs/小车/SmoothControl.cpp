/*
 * SmoothControl.cpp - 优化版双传感器循迹
 * 核心改进：
 * 1. 引入 PWM 调速：不再是生硬的 停车/开车，而是 快转/慢转
 * 2. 差速转向：转弯时，一侧减速而不是刹停，行进更丝滑
 */

// 定义引脚
const int leftMotorPin1 = 22;
const int leftMotorPin2 = 23;
// 注意：必须连接到支持 PWM 的引脚才能调速（在 STM32/Arduino 上通常标有 ~）
const int leftMotorPWM = 2;  // 假设增加一个使能/调速脚，或者 Pin22 本身支持 PWM

const int rightMotorPin1 = 24;
const int rightMotorPin2 = 25;
const int rightMotorPWM = 3; // 同上

const int sensorLeftPin = 30;
const int sensorRightPin = 32;

// 速度参数 (0-255)
const int BASE_SPEED = 150;      // 直行速度 (不要设太快，否则反应不过来)
const int TURN_FAST = 160;       // 转弯时外侧轮速度 -> 加速一点
const int TURN_SLOW = 60;        // 转弯时内侧轮速度 -> 减速但不停车 (差速)

void setup() {
    // 电机引脚初始化
    pinMode(leftMotorPin1, OUTPUT);
    pinMode(leftMotorPin2, OUTPUT);
    pinMode(leftMotorPWM, OUTPUT);

    pinMode(rightMotorPin1, OUTPUT);
    pinMode(rightMotorPin2, OUTPUT);
    pinMode(rightMotorPWM, OUTPUT);

    // 传感器引脚
    pinMode(sensorLeftPin, INPUT);
    pinMode(sensorRightPin, INPUT);
}

void loop() {
    bool isLeftBlack = (digitalRead(sensorLeftPin) == HIGH);
    bool isRightBlack = (digitalRead(sensorRightPin) == HIGH);

    if (!isLeftBlack && !isRightBlack) {
        // 情况1：都在白线上 -> 直行
        move(BASE_SPEED, BASE_SPEED);
    }
    else if (isLeftBlack && !isRightBlack) {
        // 情况2：左边压线了 -> 稍微往左修 -> 左轮慢，右轮快
        move(TURN_SLOW, TURN_FAST);
    }
    else if (!isLeftBlack && isRightBlack) {
        // 情况3：右边压线了 -> 稍微往右修 -> 左轮快，右轮慢
        move(TURN_FAST, TURN_SLOW);
    }
    else {
        // 情况4：都在黑线(十字路口) 或 都在白线外(出轨)
        // 简单处理：停车，或者保持直行冲过去
        move(0, 0); 
    }
}

/**
 * 通用电机控制函数
 * speedLeft: 左轮速度 (0-255)，负数代表反转(可选实现)
 * speedRight: 右轮速度 (0-255)
 */
void move(int speedLeft, int speedRight) {
    // 左轮控制
    if (speedLeft >= 0) {
        digitalWrite(leftMotorPin1, HIGH);
        digitalWrite(leftMotorPin2, LOW);
        analogWrite(leftMotorPWM, speedLeft); // PWM 调速
    }
    
    // 右轮控制
    if (speedRight >= 0) {
        digitalWrite(rightMotorPin1, HIGH);
        digitalWrite(rightMotorPin2, LOW);
        analogWrite(rightMotorPWM, speedRight); // PWM 调速
    }
}
