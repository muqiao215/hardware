# AIZHIGU 机械臂扩展板：接口对照 + 上位机/烧录说明

> 适用：你照片里这块 **Arduino mega328p-au / Nano** 机械臂扩展板（CG1~CG6、手柄接口、6路舵机口、RX/TX 蓝牙口）。
>
> 口的共同规律：所有 “三针口” 如果旁边写了 `GND / 5V0 / SIG`，那三排就是 **地 / 5V / 信号**（以板上丝印为准）。

---

## 1) 供电 / USB

| 位置/丝印 | 接口 | 用途 | 备注 |
|---|---|---|---|
| DC 圆孔 | DC 插座 | 给板子/舵机供电 | 不要只靠 USB 带多舵机 |
| 绿色端子 `+/-` | 接线端子 | DC 的另一种接法 | `+` 正极，`-` 负极 |
| `ON/OFF` | 电源开关 | 外部供电总开关 | |
| Micro-USB | USB | 烧录 / 串口监视器 | |

---

## 2) 6 路舵机口（顶端一排）

每个舵机口三排：`GND / 5V0 / SIG`（舵机常见线序：黑/棕=GND，红=5V，黄/白/橙=SIG）。

| 舵机通道 | SIG 对应 Arduino 引脚 |
|---|---|
| CH1 | D10 |
| CH2 | A2 |
| CH3 | A3 |
| CH4 | A0 |
| CH5 | A1 |
| CH6 | D7 |

---

## 3) CG1~CG6（传感器/扩展三针口）

CG 口本质是把“某个引脚”做成三针口：`GND / 5V0 / SIG`。

| CG 区域（按板上标注） | 该区 SIG 对应引脚 | 典型用途 |
|---|---|---|
| `-6  -11` | D6 / D11 | 数字模块、PWM/触发等 |
| `-3  -5` | D3 / D5 | 数字模块、超声波 Trig/Echo 等 |
| `A4  A5` | A4 / A5 | I2C（SDA/SCL） |
| `A6`（黄色三针） | A6 | **模拟输入**（`analogRead(A6)`），不能当普通数字口 |

---

## 4) A7 电压检测（板载功能）

| 引脚 | 用途 | 需要你接线吗 |
|---|---|---|
| A7 | 电压检测 ADC（固件里 `PIN_VOLTAGE=A7`） | **不需要**（板上已做分压） |

---

## 5) 手柄接口（PS2）

板上手柄区丝印：`3.3V GND D8 D9 D2 D4`

| PS2 信号 | 接到板子/Arduino |
|---|---|
| DAT | D8 |
| CMD | D9 |
| ATT/CS | D2 |
| CLK | D4 |
| VCC | 3.3V（优先） |
| GND | GND |

---

## 6) 蓝牙串口口（`5V0 GND RX1 TX1`）

这是给 HC-05/HC-06 这类“串口蓝牙”模块用的。

| 蓝牙模块 | 接到板子/Arduino | 备注 |
|---|---|---|
| VCC | 5V0 | |
| GND | GND | |
| TXD | RX1（D0/RX） | |
| RXD | TX1（D1/TX） | **建议分压/串电阻**，避免 5V 直灌 |

> 烧录时建议暂时拔掉蓝牙占用的 `RX1/TX1`，否则容易上传失败。

---

# 上位机与“烧录”到底是什么关系

## 你要“烧录”的只有：Arduino（机械臂主控）固件
- 机械臂接 USB（COM4），用 Arduino IDE 或 arduino-cli 把 `.ino` 上传到板子。

## “上位机”不需要烧录到 Arduino
- **PC 上位机（Windows EXE）**：直接在电脑上运行。
- **Android 上位机（APK）**：装到手机上运行（蓝牙连接）。
- **你自己的桌面上位机（robotic-arm-sequencer）**：在电脑上运行（USB 串口）。

---

# 机械臂主控固件：推荐烧录哪个

## A) 推荐：`arduino_robot_arm`（支持串口命令 + 可选 PS2）
项目路径：`E:\web\embedded\arduino_robot_arm`

### arduino-cli（Windows）一键编译+上传
如果你 `arduino-cli` 不在 PATH，直接用绝对路径：

```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" compile -b arduino:avr:nano -p COM4 -u E:\web\embedded\arduino_robot_arm
```

如果上传失败（老 Nano 引导程序），改成：
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" compile -b arduino:avr:nano:cpu=atmega328old -p COM4 -u E:\web\embedded\arduino_robot_arm
```

### 串口监视器（115200）
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" monitor -p COM4 -c baudrate=115200
```

固件启动后常用命令（在 monitor 里直接输入回车）：
- `ARM`（解锁舵机）
- `CENTER`（回中）
- `#1P1500T60!`（控制舵机：通道1，脉宽1500，时长60ms）

---

## A2) 考试程序（光敏 + 手势 I2C，无颜色传感器）
示例路径：`E:\web\embedded\arduino_robot_arm\test\EXAM_LDR_APDS9960`

### 接线
- **手势 APDS9960（I2C 0x39）**：接 `CG(A4/A5)`（`A4=SDA`、`A5=SCL`），`VCC->3.3V（优先）`，`GND->GND`
- **光敏模块（模拟阈值）**：接 `CG6(A6)` 三针（`OUT->A6`，`VCC->5V0`，`GND->GND`）
  - 说明：这块扩展板的 `A7` 默认被“电压检测分压电路”占用，**不建议当传感器口**。

### 行为（按你“只用光敏当障碍”的简化）
- SAFE START：上电默认 `DISARMED`（不输出舵机 PWM）
- 光敏触发次数：第 1 次触发开始（延时 2 秒）→ 第 2 次触发报警（可关）→ 第 3 次触发停止回 HOME 并 DISARM
- 手势：`LEFT`=张开夹爪，`RIGHT`=闭合夹爪（也可在串口里输入 `OPEN/CLOSE`）

### 校准（很关键，不校准“动作表”会不准）
1. 先把机械臂放到你认为的 HOME 位，在串口输入 `ARM`，再输入 `POSE GO HOME 1500`（慢慢去 HOME）。
2. 如果 HOME 不对：用 `POSE SET HOME p1 p2 p3 p4 p5 p6` 直接改 6 路 PWM（500~2500）。
3. 同理改 `PICK_1/2/3` 与 `DROP_A/B/C`，每次改完可 `SAVE` 写入 EEPROM，断电不丢。
4. 夹爪（6轴）单独先定开/闭：
   - `GRIP OPEN 1600`（张开）
   - `GRIP CLOSE 1400`（夹紧）
   - 运行中也可以直接输入 `OPEN`/`CLOSE` 立即动作（同时满足等待步骤）

### 编译/烧录
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" compile -b arduino:avr:nano -p COM4 -u E:\web\embedded\arduino_robot_arm\test\EXAM_LDR_APDS9960
```

串口监视器：
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" monitor -p COM4 -c baudrate=115200
```
常用命令：`HELP`、`ARM`、`START`、`STOP`、`STARTTH 600`、`INVSTART 1`、`OBSTACLE off`、`GRIP OPEN 1600`、`GRIP CLOSE 1400`

如果要把示教的 `111.tox` 点位“写进程序里”并播放：
- 串口输入 `MODE TOX`（切换到脚本模式）
- 然后输入 `TOX RUN`（立即开始播放 26 步）

---

## A3) 纯烧录验证：TOX111 自动播放（不使用串口）
示例路径：`E:\web\embedded\arduino_robot_arm\test\TOX111_AUTORUN`

特点：不需要打开串口、也不需要输入任何命令；上电后等待光敏 A6 触发，延时 2 秒，自动播放 `111.tox` 的 26 步，然后停机并断开舵机 PWM。

### 编译/烧录
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" compile -b arduino:avr:nano -p COM4 -u E:\web\embedded\arduino_robot_arm\test\TOX111_AUTORUN
```

---

## A4) 最终版：最终版.tox 自动运行（不使用串口）
示例路径：`E:\web\embedded\arduino_robot_arm\test\FINAL_AUTORUN`

特点（按实操题逻辑）：
- 光敏 A6：**第一次触发** → 等待 2 秒 → 开始循环运行；**第二次触发** → 停止运行（回中并断开 PWM）
- 超声波 HC-SR04：遇到障碍立即停机报警（LED 1Hz 闪烁 + 蜂鸣器 1Hz），串口收到 `start` 才恢复
- 说明：障碍物移开后会自动**停止蜂鸣器**（仍保持暂停，等待串口 `start` 恢复运行）
- 手势 APDS9960：运行/报警时都可用手势控制夹爪（LEFT/UP=开，RIGHT/DOWN=关）
- Serial Studio：可选（CSV 遥测 115200），用于看当前 step/传感器/6轴脉宽

### 接线
- 舵机（CH1..CH6）：`D10, A2, A3, A0, A1, D7`（不变）
- 手势 APDS9960（I2C 0x39）：接 `CG(A4/A5)`（`A4=SDA`、`A5=SCL`），`VCC->3.3V（优先）`，`GND->GND`
- 光敏模块（模拟阈值）：接 `CG6(A6)` 三针（`OUT->A6`，`VCC->5V0`，`GND->GND`）
- HC-SR04 超声波：`VCC->5V0`，`GND->GND`，`Trig->D3`（CG `-3`），`Echo->D5`（CG `-5`）

### 行为/参数（在代码里改，无需串口）
打开 `arduino_robot_arm/test/FINAL_AUTORUN/FINAL_AUTORUN.ino`：
- `kLdrSignalThreshold / kLdrSignalActiveLow`：光敏触发阈值与方向（一般是“遮挡/灯灭”=低）
- `kStartDelayMs`：第一次触发后等待多久开始（默认 2000ms）
- `kObstacleCm`：超声波遇障距离（cm）
- `kGripOpenUs / kGripCloseUs`：夹爪开/关 PWM（us）

### Serial Studio（可选，串口可视化）
`FINAL_AUTORUN` 会以 **CSV** 输出一行遥测（默认 10Hz）：
`ms,state,step,ldr,base,delta,cm,s1,s2,s3,s4,s5,s6`

Serial Studio 操作（最短流程）：
1. 打开 Serial Studio → 选择 `COM4` → Baud 选 `115200` → Connect
2. 新建/编辑 Project：选择 **CSV**（分隔符 `,`，帧结束 `\\n`）
3. 添加图表：把 `step`、`ldr`、`delta`、`cm`、`s1..s6` 作为曲线即可

注意：烧录前请关闭 Serial Studio（否则 COM4 被占用会上传失败）。恢复报警用的串口命令就是在 Serial Studio/任意串口工具里发送一行：`start`

### 编译/烧录
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" compile -b arduino:avr:nano -p COM4 -u E:\web\embedded\arduino_robot_arm\test\FINAL_AUTORUN
```

如果是老 Nano 引导程序（上传失败），用：
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" compile -b arduino:avr:nano:cpu=atmega328old -p COM4 -u E:\web\embedded\arduino_robot_arm\test\FINAL_AUTORUN
```

> 如果提示 `Access denied` / `Serial port busy`：说明 COM4 被占用（例如 arduino-cli monitor / Arduino IDE 串口监视器开着），关掉再烧录。

---

## B) 供应商“PC上位机/PS2_BT”配套示例（如果你要完全对齐原厂例程）
示例路径（你资源包里）：
`E:\web\embedded\arduino_robot_arm\resources\arduino版本开发资料\3.由浅入深控制例程源码\4.由浅入深控制例程源码\16.PC图形化上位机在线控制机械臂\ps2_bt\PS2_Servor`

上传命令：
```powershell
& "C:\Program Files\Arduino CLI\arduino-cli.exe" compile -b arduino:avr:nano -p COM4 -u "E:\web\embedded\arduino_robot_arm\resources\arduino版本开发资料\3.由浅入深控制例程源码\4.由浅入深控制例程源码\16.PC图形化上位机在线控制机械臂\ps2_bt\PS2_Servor"
```

---

# 上位机怎么“安装/运行”

## 1) Android 上位机（`android_arm_controller`）
工程路径：`E:\web\embedded\android_arm_controller`

构建 APK（Windows）：
```powershell
cd E:\web\embedded\android_arm_controller
.\gradlew.bat assembleDebug
```

输出 APK：
`E:\web\embedded\android_arm_controller\app\build\outputs\apk\debug\app-debug.apk`

安装到手机（可选，需要 adb）：
```powershell
adb install -r E:\web\embedded\android_arm_controller\app\build\outputs\apk\debug\app-debug.apk
```

## 2) PC 上位机（你的桌面软件：`robotic-arm-sequencer`）
工程路径：`E:\web\robotic-arm-sequencer`

开发模式启动：
```powershell
cd E:\web\robotic-arm-sequencer
npm run electron:dev
```

> 这类上位机运行在电脑上，通过 USB 串口发送 `#...!` 命令控制机械臂；Arduino 端仍然只需要烧录固件（上面 A/B 任选其一）。
