# 📚 STM32 核心概念知识库

这里存放了嵌入式开发中最底层的理论基础。建议按照顺序阅读。

## 📖 目录

### [01. C 语言基石](./01_C_Language_Foundations.md)
*   进制转换 (Hex/Bin)
*   位运算详解 (&, |, ^, ~, <<)
*   结构体与指针在寄存器映射中的应用
*   `volatile` 关键字的重要性

### [02. GPIO 内部架构](./02_GPIO_Internal_Architecture.md)
*   保护二极管与上下拉电阻
*   施密特触发器 (Schmitt Trigger) 原理
*   推挽输出 vs 开漏输出 (P-MOS/N-MOS)
*   8 种工作模式详解

### [03. GPIO 编程实战](./03_GPIO_Coding_Practices.md)
*   位掩码 (Bitmask) 的组合技巧
*   标准库函数 (`SetBits`, `Write`) 的区别与应用
*   灌电流 (Sink) vs 拉电流 (Source)
*   流水灯与蜂鸣器的控制逻辑

---
> **提示**: 这些理论是写出高质量代码的基石。遇到“为什么代码是对的但灯不亮”这种问题时，多半能在这里找到答案。
