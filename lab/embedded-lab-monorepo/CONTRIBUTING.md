# Contributing

这个仓库已经不是单一课程代码目录，而是一个包含四条独立主线的嵌入式 monorepo。

提交任何改动前，先接受这几个前提：

- 这是一个 monorepo，不是一个共享构建系统的大工程
- `STM32F103`、`mcs51`、`rpi_hpad`、`WM8960-Audio-HAT` 四条线共用一个 git 仓库，但各自独立演进
- 当前软件侧可以持续推进，硬件侧必须诚实标注 pending
- 新增内容不优先，边界清楚、可验证、可交接优先

## 仓库边界

### STM32F103

主要目录：

- `boards/`
- `common/`
- `examples/`
- `cmake/`
- `tests/`
- `scripts/`

约束：

- `common/` 只放平台层、驱动层、组件层和纯逻辑工具
- `boards/` 只放板级映射、时钟、引脚别名、内存布局和板级说明
- `examples/` 只放示例装配层，不要回长驱动
- 不要把板级细节重新写回示例

### MCS-51 / STC89C52

主要目录：

- `mcs51/common/`
- `mcs51/examples/`
- `mcs51/tests/`
- `mcs51/scripts/`

约束：

- 优先保持 `SDCC` 原生风格，不做厚重的 Keil 魔法兼容层
- 示例保持最小，逻辑尽量沉到 `common/`
- 能主机验证的逻辑，不要等真板
- 真板相关风险优先写进 `mcs51/HARDWARE_PENDING.md`

### Raspberry Pi HPAD

主要目录：

- `rpi_hpad/drivers/`
- `rpi_hpad/services/`
- `rpi_hpad/app/`
- `rpi_hpad/tests/`
- `rpi_hpad/scripts/`

约束：

- 上层逻辑建立在统一 board interface 上
- `mock`、`replay`、`real` 三个 backend 必须边界清楚
- `real` backend 在没有真板前只保留配置和诚实错误边界
- 不要把不可验证的硬件假设包装成“已经支持”

### WM8960 Audio HAT

主要目录：

- `WM8960-Audio-HAT/`

约束：

- 这是 vendor-imported 资源，优先保留原始结构
- 除非有明确维护需求，不要随手大改上游驱动代码
- 如果补本仓库说明，尽量放在根 README 或新增薄包装文档里
- 安装脚本涉及系统修改，变更前先明确影响范围

## 提交前必须跑什么

全仓提交前，默认跑：

```bash
scripts/check-all.sh
```

它会顺序跑：

- `scripts/check.sh`
- `mcs51/scripts/check.sh`
- `rpi_hpad/scripts/check.sh`

如果你只改其中一条线，至少跑对应 gate：

```bash
scripts/check.sh
cd mcs51 && scripts/check.sh
cd rpi_hpad && scripts/check.sh
```

如果改动触及纯逻辑模块，必须保证主机侧测试继续通过。

## 文档和发布面

涉及行为变化、边界变化、版本冻结点变化时，顺手更新对应文档：

- 根目录：`README.md`, `CHANGELOG.md`, `RELEASE_NOTES.md`
- STM32：`HARDWARE_PENDING.md`, `REALBOARD_VALIDATION_CHECKLIST.md`
- MCS-51：`mcs51/HARDWARE_PENDING.md`, 示例 README
- RPi HPAD：`rpi_hpad/QUICKSTART.md`, `rpi_hpad/PROTOCOL.md`, `rpi_hpad/CHANGELOG.md`, `rpi_hpad/RELEASE_NOTES.md`
- WM8960 Audio HAT：优先更新 `WM8960-Audio-HAT/README.md` 或根目录入口说明，避免把 vendor 目录改成另一套风格

如果一个变更已经影响到“别人怎么接手”或“别人怎么验证”，只改代码不改文档，视为未完成。

## 不要做什么

- 不要把四条线强行揉成一个构建系统
- 不要把 legacy 课程资料删掉
- 不要宣称任何真板结果，除非已经在真实硬件上验证并记录
- 不要把示例重新做厚，示例应该越来越像装配层
- 不要把板级映射、驱动实现、器件逻辑、纯逻辑工具混写
- 不要为了“看起来更完整”补不可验证的假驱动
- 不要无必要重写 vendor 音频驱动，只因为代码风格不统一

## 硬件声明红线

以下内容在没有真板记录前，一律不能写成 done：

- STM32 已经成功烧录、成功点亮、成功串口交互
- STC89C52 已经成功烧录、成功串口输出、成功按键验证
- ADS1256 / DAC8532 已经完成 SPI/GPIO/精度验证
- WM8960 音频输入输出链路在某个提交里已经被完整复核，除非你有新的实测证据

正确写法应该是：

- `hardware_pending`
- `real backend not implemented`
- `awaiting board-side validation`

## 推荐提交流程

1. 先确认改动属于哪条主线
2. 改代码时守住边界，不把逻辑写回示例
3. 跑对应子项目 gate
4. 跑 `scripts/check-all.sh`
5. 补最小必要文档
6. 再提交

## 提交信息建议

优先使用清楚、窄范围的提交信息，例如：

- `feat: add monorepo aggregate gate`
- `docs: polish monorepo landing page`
- `fix: keep cli input pump state across calls`
- `test: expand ringbuf boundary coverage`

如果变更只影响某条线，信息里最好体现作用域，例如：

- `feat(mcs51): add host-side debounce tests`
- `docs(rpi_hpad): freeze event protocol wording`
- `fix(stm32): split board mapping from example init`

## 最后原则

这个仓库的主线不是“继续堆功能”，而是：

- 让示例更薄
- 让平台更稳
- 让验证更硬
- 让交接更顺

如果一个改动会让仓库更难理解、更难验证、更难交接，那它大概率就不该现在合进去。
