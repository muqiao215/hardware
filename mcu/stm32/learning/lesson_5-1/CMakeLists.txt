cmake_minimum_required(VERSION 3.20)

# ============================================================================
# 通用嵌入式 CMake 工程模板
# 适用于: STM32 / GD32 / NRF / 其他ARM Cortex-M芯片
# ============================================================================

project(embedded_firmware C CXX ASM)

# 语言标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# 必须指定工具链文件
# ============================================================================
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR
        "Please specify toolchain file:\n"
        "  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake"
    )
endif()

# ============================================================================
# 必须指定目标板卡
# ============================================================================
set(BOARD "" CACHE STRING "Target board configuration name")
if(NOT BOARD)
    message(FATAL_ERROR
        "Please specify target board:\n"
        "  cmake -S . -B build ... -DBOARD=stm32f103c8t6\n"
        "Available boards in cmake/boards/:\n"
        "  - stm32f103c8t6 (STM32F103C8T6 Blue Pill)\n"
        "  - Add more boards as needed"
    )
endif()

message(STATUS "========================================")
message(STATUS "Building firmware for board: ${BOARD}")
message(STATUS "========================================")

# ============================================================================
# 加载板卡配置 (会定义: LINKER_SCRIPT, STARTUP, BOARD_DEFINES等)
# ============================================================================
include(cmake/boards/${BOARD}.cmake)

# ============================================================================
# 应用程序选择 (多程序支持)
# ============================================================================
set(APP "" CACHE STRING "Application to build (leave empty to list available)")

# 自动扫描 src/main_*.c 文件，生成可用程序列表
file(GLOB APP_MAIN_FILES "${CMAKE_SOURCE_DIR}/src/main_*.c")
set(AVAILABLE_APPS "")
foreach(APP_FILE ${APP_MAIN_FILES})
    get_filename_component(APP_NAME ${APP_FILE} NAME_WE)
    string(REPLACE "main_" "" APP_NAME ${APP_NAME})
    list(APPEND AVAILABLE_APPS ${APP_NAME})
endforeach()

# 如果存在 src/main.c，添加 "default" 选项
if(EXISTS "${CMAKE_SOURCE_DIR}/src/main.c")
    list(APPEND AVAILABLE_APPS "default")
endif()

# 显示可用程序或验证选择
if(NOT APP)
    string(REPLACE ";" "\n    - " APP_LIST "${AVAILABLE_APPS}")
    message(FATAL_ERROR
        "Please specify application to build:\n"
        "  cmake -S . -B build ... -DAPP=<app_name>\n\n"
        "Available applications:\n    - ${APP_LIST}\n\n"
        "Example:\n"
        "  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake -DBOARD=stm32f103c8t6 -DAPP=ir_sensor -G Ninja"
    )
endif()

# 根据 APP 选择确定 main 文件
if(APP STREQUAL "default")
    set(APP_MAIN_SRC "${CMAKE_SOURCE_DIR}/src/main.c")
else()
    set(APP_MAIN_SRC "${CMAKE_SOURCE_DIR}/src/main_${APP}.c")
endif()

# 验证文件存在
if(NOT EXISTS ${APP_MAIN_SRC})
    string(REPLACE ";" ", " APP_LIST "${AVAILABLE_APPS}")
    message(FATAL_ERROR
        "Application '${APP}' not found!\n"
        "  Expected file: ${APP_MAIN_SRC}\n"
        "  Available: ${APP_LIST}"
    )
endif()

message(STATUS "Application: ${APP}")
message(STATUS "Main source: ${APP_MAIN_SRC}")

# ============================================================================
# 公共模块 (所有程序共享)
# ============================================================================
set(COMMON_SOURCES
    src/Delay.c
    src/OLED.c
    src/CountSensor.c
)

# ============================================================================
# 应用程序主目标
# ============================================================================
add_executable(firmware.elf
    ${APP_MAIN_SRC}
    ${COMMON_SOURCES}
    ${STARTUP}  # 由 board.cmake 定义
)

# 链接板卡相关的库 (CMSIS, HAL等)
target_link_libraries(firmware.elf PRIVATE
    cmsis      # CMSIS Core + 系统初始化
    hal        # STM32标准外设库/HAL库
)

# 包含目录
target_include_directories(firmware.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 板卡特定的宏定义
target_compile_definitions(firmware.elf PRIVATE
    ${BOARD_DEFINES}
)

# 板卡特定的编译选项
target_compile_options(firmware.elf PRIVATE
    ${BOARD_C_FLAGS}
)

# 链接选项: 链接脚本 + 生成map文件
target_link_options(firmware.elf PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_BINARY_DIR}/firmware.map
    -Wl,--print-memory-usage
    ${BOARD_LINK_FLAGS}
)

# ============================================================================
# 生成二进制文件 (bin/hex) 和打印固件大小
# ============================================================================
add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware.elf> ${CMAKE_BINARY_DIR}/firmware.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:firmware.elf> ${CMAKE_BINARY_DIR}/firmware.hex
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:firmware.elf>
    COMMENT "Generating binary files and showing size..."
)

# ============================================================================
# 自定义烧录目标 (可选 - 需要 OpenOCD)
# ============================================================================
if(DEFINED OPENOCD_CFG)
    add_custom_target(flash
        COMMAND openocd -f ${OPENOCD_CFG}
                -c "program ${CMAKE_BINARY_DIR}/firmware.elf verify reset exit"
        DEPENDS firmware.elf
        COMMENT "Flashing firmware to target..."
    )
endif()

# ============================================================================
# 显示配置摘要
# ============================================================================
message(STATUS "========================================")
message(STATUS "Configuration Summary:")
message(STATUS "  Application:    ${APP}")
message(STATUS "  Board:          ${BOARD}")
message(STATUS "  Toolchain:      ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "  Linker Script:  ${LINKER_SCRIPT}")
message(STATUS "  Startup File:   ${STARTUP}")
message(STATUS "========================================")
