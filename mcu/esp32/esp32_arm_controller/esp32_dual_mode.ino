/**
 * ESP32 双模固件 - WiFi + BLE 机械臂控制器
 * 
 * 功能:
 * 1. BLE GATT 服务 (Nordic UART Service 兼容)
 * 2. WiFi WebSocket 服务器
 * 3. 兼容原有串口指令格式 (#1P1500T100!)
 * 4. 双模同时运行
 * 
 * 引脚配置 (与 Arduino 机械臂一致):
 * - Servo 1: GPIO 10
 * - Servo 2: GPIO A2 (GPIO 36)
 * - Servo 3: GPIO A3 (GPIO 39)
 * - Servo 4: GPIO A0 (GPIO 34)
 * - Servo 5: GPIO A1 (GPIO 35)
 * - Servo 6: GPIO 7
 */

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>

// ========== WiFi 配置 ==========
const char* WIFI_SSID = "RobotArm_AP";     // AP 模式 SSID
const char* WIFI_PASSWORD = "12345678";    // AP 模式密码
const int WEBSOCKET_PORT = 81;

// ========== BLE 配置 (Nordic UART Service 兼容) ==========
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// ========== 舵机配置 ==========
const int SERVO_PINS[6] = {10, 36, 39, 34, 35, 7};  // ESP32 引脚
const int SERVO_MIN = 500;   // 最小脉宽 (us)
const int SERVO_MAX = 2500;  // 最大脉宽 (us)

Servo servos[6];
int servoPositions[6] = {1500, 1500, 1500, 1500, 1500, 1500};  // 初始位置 (中位)

// ========== 全局对象 ==========
WebSocketsServer webSocket(WEBSOCKET_PORT);
BLEServer* pServer = nullptr;
BLECharacteristic* pTxCharacteristic = nullptr;
bool bleDeviceConnected = false;
bool oldBleDeviceConnected = false;

// ========== BLE 回调 ==========
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        bleDeviceConnected = true;
        Serial.println("[BLE] Device connected");
    }
    
    void onDisconnect(BLEServer* pServer) {
        bleDeviceConnected = false;
        Serial.println("[BLE] Device disconnected");
    }
};

class MyRxCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        String rxValue = pCharacteristic->getValue().c_str();
        if (rxValue.length() > 0) {
            Serial.print("[BLE] Received: ");
            Serial.println(rxValue);
            processCommand(rxValue);
        }
    }
};

// ========== WebSocket 回调 ==========
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WS] Client %u disconnected\n", num);
            break;
            
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[WS] Client %u connected from %s\n", num, ip.toString().c_str());
            webSocket.sendTXT(num, "{\"status\":\"connected\"}");
            break;
        }
        
        case WStype_TEXT:
            Serial.printf("[WS] Received: %s\n", payload);
            processCommand(String((char*)payload));
            break;
    }
}

// ========== 指令解析 ==========
void processCommand(String cmd) {
    cmd.trim();
    
    // 支持原有格式: #1P1500T100!
    if (cmd.startsWith("#") && cmd.endsWith("!")) {
        parseServoCommand(cmd);
    }
    // 支持 JSON 格式: {"servo":1,"pwm":1500,"time":100}
    else if (cmd.startsWith("{")) {
        parseJsonCommand(cmd);
    }
    // 特殊指令
    else if (cmd == "STATUS") {
        sendStatus();
    }
    else if (cmd == "CENTER") {
        centerAll();
    }
}

void parseServoCommand(String cmd) {
    // 格式: #<ID>P<PWM>T<TIME>!
    // 例如: #1P1500T100!
    
    int idStart = cmd.indexOf('#') + 1;
    int pPos = cmd.indexOf('P');
    int tPos = cmd.indexOf('T');
    int endPos = cmd.indexOf('!');
    
    if (idStart > 0 && pPos > idStart && tPos > pPos && endPos > tPos) {
        int servoId = cmd.substring(idStart, pPos).toInt();
        int pwm = cmd.substring(pPos + 1, tPos).toInt();
        int duration = cmd.substring(tPos + 1, endPos).toInt();
        
        if (servoId >= 1 && servoId <= 6) {
            pwm = constrain(pwm, SERVO_MIN, SERVO_MAX);
            moveServo(servoId - 1, pwm, duration);
        }
    }
}

void parseJsonCommand(String cmd) {
    // 简单 JSON 解析 (避免引入 ArduinoJson 依赖)
    // 格式: {"servo":1,"pwm":1500,"time":100}
    
    int servoIdx = cmd.indexOf("\"servo\":");
    int pwmIdx = cmd.indexOf("\"pwm\":");
    int timeIdx = cmd.indexOf("\"time\":");
    
    if (servoIdx > 0 && pwmIdx > 0 && timeIdx > 0) {
        int servoId = getJsonValue(cmd, servoIdx + 8);
        int pwm = getJsonValue(cmd, pwmIdx + 6);
        int duration = getJsonValue(cmd, timeIdx + 7);
        
        if (servoId >= 1 && servoId <= 6) {
            pwm = constrain(pwm, SERVO_MIN, SERVO_MAX);
            moveServo(servoId - 1, pwm, duration);
        }
    }
}

int getJsonValue(String& json, int startIdx) {
    int endIdx = startIdx;
    while (endIdx < json.length() && (isDigit(json[endIdx]) || json[endIdx] == '-')) {
        endIdx++;
    }
    return json.substring(startIdx, endIdx).toInt();
}

// ========== 舵机控制 ==========
void moveServo(int idx, int targetPwm, int duration) {
    if (idx < 0 || idx >= 6) return;
    
    int currentPwm = servoPositions[idx];
    int steps = abs(targetPwm - currentPwm);
    
    if (steps == 0 || duration <= 0) {
        servos[idx].writeMicroseconds(targetPwm);
        servoPositions[idx] = targetPwm;
        return;
    }
    
    // 平滑移动
    int stepDelay = duration / steps;
    if (stepDelay < 1) stepDelay = 1;
    
    int direction = (targetPwm > currentPwm) ? 1 : -1;
    
    for (int i = 0; i < steps; i++) {
        currentPwm += direction;
        servos[idx].writeMicroseconds(currentPwm);
        delayMicroseconds(stepDelay * 1000);
    }
    
    servos[idx].writeMicroseconds(targetPwm);
    servoPositions[idx] = targetPwm;
}

void centerAll() {
    for (int i = 0; i < 6; i++) {
        moveServo(i, 1500, 500);
    }
    Serial.println("[CMD] All servos centered");
}

void sendStatus() {
    String status = "{\"servos\":[";
    for (int i = 0; i < 6; i++) {
        status += String(servoPositions[i]);
        if (i < 5) status += ",";
    }
    status += "],\"ble\":";
    status += bleDeviceConnected ? "true" : "false";
    status += ",\"wifi\":";
    status += (WiFi.softAPgetStationNum() > 0) ? "true" : "false";
    status += "}";
    
    // 通过 BLE 发送
    if (bleDeviceConnected && pTxCharacteristic) {
        pTxCharacteristic->setValue(status.c_str());
        pTxCharacteristic->notify();
    }
    
    // 通过 WebSocket 广播
    webSocket.broadcastTXT(status);
    
    Serial.print("[STATUS] ");
    Serial.println(status);
}

// ========== 初始化 ==========
void setupServos() {
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    for (int i = 0; i < 6; i++) {
        servos[i].setPeriodHertz(50);
        servos[i].attach(SERVO_PINS[i], SERVO_MIN, SERVO_MAX);
        servos[i].writeMicroseconds(1500);  // 初始中位
    }
    
    Serial.println("[SERVO] All servos initialized");
}

void setupWiFi() {
    // AP 模式 (设备作为热点)
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("[WiFi] AP Mode - IP: ");
    Serial.println(IP);
    
    // 启动 WebSocket 服务器
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.printf("[WiFi] WebSocket server started on port %d\n", WEBSOCKET_PORT);
}

void setupBLE() {
    BLEDevice::init("RobotArm_ESP32");
    
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    BLEService* pService = pServer->createService(SERVICE_UUID);
    
    // TX Characteristic (Notify)
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pTxCharacteristic->addDescriptor(new BLE2902());
    
    // RX Characteristic (Write)
    BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE
    );
    pRxCharacteristic->setCallbacks(new MyRxCallbacks());
    
    pService->start();
    
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    Serial.println("[BLE] Service started, advertising...");
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n========================================");
    Serial.println("  ESP32 Robot Arm - Dual Mode Controller");
    Serial.println("  WiFi + BLE");
    Serial.println("========================================\n");
    
    setupServos();
    setupWiFi();
    setupBLE();
    
    Serial.println("\n[READY] System initialized");
    Serial.println("  - WiFi SSID: " + String(WIFI_SSID));
    Serial.println("  - BLE Name: RobotArm_ESP32");
    Serial.println("  - Command format: #<ID>P<PWM>T<TIME>!");
}

void loop() {
    // 处理 WebSocket
    webSocket.loop();
    
    // 处理 BLE 重连
    if (!bleDeviceConnected && oldBleDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        Serial.println("[BLE] Restart advertising");
        oldBleDeviceConnected = bleDeviceConnected;
    }
    if (bleDeviceConnected && !oldBleDeviceConnected) {
        oldBleDeviceConnected = bleDeviceConnected;
    }
    
    // 处理串口输入 (调试用)
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        processCommand(cmd);
    }
}
