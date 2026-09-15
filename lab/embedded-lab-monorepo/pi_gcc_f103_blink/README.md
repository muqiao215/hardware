# pi_gcc_f103_blink

这是从原仓库的 `2-2 库函数方式` 演化出来的最小 GNU 工程，目标板卡是 `STM32F103C8T6 / Blue Pill`。

用途：

- 在树莓派上无屏编译
- 使用 `ST-Link` 烧录
- 验证 OLED 初始化与表情动画显示

## 构建

```bash
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake
cmake --build build
```

## 烧录

先把 ST-Link 插到树莓派，再执行：

```bash
cmake --build build --target flash
```

当前 `flash` 目标底层调用的是 `st-flash write ... 0x08000000`。

## 现象

- 上电后初始化 OLED
- 程序循环显示笑脸和哭脸动画
- 这是当前主工程的最小演示，不再是 blink-only 工程
