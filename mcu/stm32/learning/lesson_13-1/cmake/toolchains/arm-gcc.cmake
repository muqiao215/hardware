# ============================================================================
# ARM GCC 工具链配置文件 (通用)
# 适用于所有 ARM Cortex-M 芯片 (STM32/GD32/NRF等)
# ============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ============================================================================
# 查找 ARM GCC 工具链
# ============================================================================
find_program(ARM_CC  arm-none-eabi-gcc REQUIRED)
find_program(ARM_CXX arm-none-eabi-g++ REQUIRED)
find_program(ARM_ASM arm-none-eabi-gcc REQUIRED)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_OBJDUMP arm-none-eabi-objdump REQUIRED)
find_program(ARM_SIZE arm-none-eabi-size REQUIRED)
find_program(ARM_GDB arm-none-eabi-gdb)

set(CMAKE_C_COMPILER   ${ARM_CC})
set(CMAKE_CXX_COMPILER ${ARM_CXX})
set(CMAKE_ASM_COMPILER ${ARM_ASM})
set(CMAKE_OBJCOPY      ${ARM_OBJCOPY})
set(CMAKE_OBJDUMP      ${ARM_OBJDUMP})
set(CMAKE_SIZE         ${ARM_SIZE})

# 禁用编译器测试 (裸机工程无法生成可执行文件)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ============================================================================
# ARM 核心参数 (由 board.cmake 覆盖)
# ============================================================================
set(ARM_CPU "cortex-m3" CACHE STRING "ARM CPU core (e.g., cortex-m3, cortex-m4)")
set(ARM_FPU "" CACHE STRING "ARM FPU type (e.g., fpv4-sp-d16, leave empty if no FPU)")
set(ARM_FLOAT_ABI "soft" CACHE STRING "Float ABI (soft, softfp, hard)")

# ============================================================================
# 通用编译选项
# ============================================================================
set(COMMON_FLAGS "")
list(APPEND COMMON_FLAGS
    -mcpu=${ARM_CPU}
    -mthumb
)

# 如果有硬件浮点单元
if(ARM_FPU)
    list(APPEND COMMON_FLAGS
        -mfpu=${ARM_FPU}
        -mfloat-abi=${ARM_FLOAT_ABI}
    )
endif()

# 代码优化相关
list(APPEND COMMON_FLAGS
    -ffunction-sections   # 每个函数独立段(便于链接器GC)
    -fdata-sections       # 每个数据独立段
    -fno-common           # 避免common block
    -fstack-usage         # 生成栈使用信息
)

# 调试信息
list(APPEND COMMON_FLAGS
    -g3                   # 包含宏定义的调试信息
)

# ============================================================================
# C 编译选项
# ============================================================================
set(CMAKE_C_FLAGS_INIT "")
foreach(flag ${COMMON_FLAGS})
    set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} ${flag}")
endforeach()

set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -std=gnu11 -Wall -Wextra")

# ============================================================================
# C++ 编译选项
# ============================================================================
set(CMAKE_CXX_FLAGS_INIT "")
foreach(flag ${COMMON_FLAGS})
    set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} ${flag}")
endforeach()

set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -std=gnu++17 -Wall -Wextra")

# 裸机C++常用选项
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# ============================================================================
# 汇编编译选项
# ============================================================================
set(CMAKE_ASM_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")

# ============================================================================
# 链接选项
# ============================================================================
set(CMAKE_EXE_LINKER_FLAGS_INIT "")
list(APPEND CMAKE_EXE_LINKER_FLAGS_INIT
    -Wl,--gc-sections     # 删除未使用的段
    -Wl,--print-memory-usage  # 打印内存使用情况
    --specs=nano.specs    # 使用newlib-nano(减小代码体积)
    --specs=nosys.specs   # 不链接syscalls实现
)

# 转换为字符串
string(REPLACE ";" " " CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT}")

# ============================================================================
# 构建类型默认选项
# ============================================================================
set(CMAKE_C_FLAGS_DEBUG "-Og -DDEBUG" CACHE STRING "C Debug flags")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "C Release flags")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG" CACHE STRING "C MinSizeRel flags")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "CXX Debug flags")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "CXX Release flags")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}" CACHE STRING "CXX MinSizeRel flags")

# ============================================================================
# 提示信息
# ============================================================================
message(STATUS "ARM GCC Toolchain loaded:")
message(STATUS "  Compiler: ${ARM_CC}")
message(STATUS "  CPU:      ${ARM_CPU}")
message(STATUS "  FPU:      ${ARM_FPU}")
message(STATUS "  Float ABI: ${ARM_FLOAT_ABI}")
