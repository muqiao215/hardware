# ============================================================================
# STM32F103C8T6 (Blue Pill) 板卡配置
# 适配 STM32F10x_StdPeriph_Lib_V3.5.0 目录结构
# ============================================================================

# ============================================================================
# 芯片核心参数
# ============================================================================
set(ARM_CPU "cortex-m3" CACHE STRING "ARM CPU" FORCE)
set(ARM_FPU "" CACHE STRING "No FPU on Cortex-M3" FORCE)
set(ARM_FLOAT_ABI "soft" CACHE STRING "Soft float" FORCE)

# ============================================================================
# 芯片型号定义
# ============================================================================
set(BOARD_DEFINES
    STM32F10X_MD            # Medium Density (64KB Flash)
    USE_STDPERIPH_DRIVER    # 使用标准外设库
    HSE_VALUE=8000000       # 外部晶振8MHz
)

# ============================================================================
# 优化级别
# ============================================================================
set(BOARD_C_FLAGS
    -Os                     # 优化代码大小
)

# ============================================================================
# 链接脚本和启动文件
# ============================================================================
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/boards/stm32f103c8t6/STM32F103C8Tx_FLASH.ld)
# 使用库自带的 GCC 启动文件 (Medium Density)
set(STARTUP ${CMAKE_SOURCE_DIR}/drivers/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s)

# ============================================================================
# OpenOCD 配置 (用于烧录和调试)
# ============================================================================
set(OPENOCD_CFG ${CMAKE_SOURCE_DIR}/boards/stm32f103c8t6/openocd.cfg)

# ============================================================================
# 驱动库路径 (STM32F10x_StdPeriph_Lib_V3.5.0 目录结构)
# ============================================================================
set(STM32_STDPERIPH_DIR ${CMAKE_SOURCE_DIR}/drivers/STM32F10x_StdPeriph_Driver)
set(CMSIS_DIR           ${CMAKE_SOURCE_DIR}/drivers/CMSIS)
set(CMSIS_DEVICE_DIR    ${CMSIS_DIR}/CM3/DeviceSupport/ST/STM32F10x)
set(CMSIS_CORE_DIR      ${CMSIS_DIR}/CM3/CoreSupport)

if(NOT EXISTS ${STM32_STDPERIPH_DIR})
    message(FATAL_ERROR "STM32 StdPeriph not found at ${STM32_STDPERIPH_DIR}")
endif()

if(NOT EXISTS ${CMSIS_DIR})
    message(FATAL_ERROR "CMSIS not found at ${CMSIS_DIR}")
endif()

# ============================================================================
# CMSIS 库 (Core + System)
# 注意: 不编译 core_cm3.c，因为它与新版 GCC 有兼容性问题
# core_cm3.h 中的内联函数已经足够使用
# ============================================================================
add_library(cmsis OBJECT
    ${CMSIS_DEVICE_DIR}/system_stm32f10x.c
)

target_include_directories(cmsis PUBLIC
    ${CMSIS_CORE_DIR}
    ${CMSIS_DEVICE_DIR}
    ${STM32_STDPERIPH_DIR}/inc
)

target_compile_definitions(cmsis PUBLIC ${BOARD_DEFINES})
target_compile_options(cmsis PRIVATE ${BOARD_C_FLAGS})

# ============================================================================
# STM32F10x 标准外设库
# ============================================================================
file(GLOB STM32_PERIPH_SRC
    ${STM32_STDPERIPH_DIR}/src/*.c
)

# 排除template文件
list(FILTER STM32_PERIPH_SRC EXCLUDE REGEX ".*_template\\.c$")

add_library(hal OBJECT
    ${STM32_PERIPH_SRC}
)

target_include_directories(hal PUBLIC
    ${STM32_STDPERIPH_DIR}/inc
    ${CMSIS_CORE_DIR}
    ${CMSIS_DEVICE_DIR}
)

target_compile_definitions(hal PUBLIC ${BOARD_DEFINES})
target_compile_options(hal PRIVATE ${BOARD_C_FLAGS})

# ============================================================================
# 板卡信息
# ============================================================================
message(STATUS "Board: STM32F103C8T6 Blue Pill")
message(STATUS "  Flash: 64KB")
message(STATUS "  RAM:   20KB")
message(STATUS "  Core:  Cortex-M3 @ 72MHz (max)")
