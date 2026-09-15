cmake_minimum_required(VERSION 3.20)

project(pi_gcc_f103_blink C ASM)

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Specify -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake")
endif()

add_executable(firmware
    src/main.c
    src/oled.c
    src/stm32f10x_it.c
    startup/startup_stm32f103c8t6.s
    drivers/CMSIS/system_stm32f10x.c
    drivers/StdPeriph/misc.c
    drivers/StdPeriph/stm32f10x_gpio.c
    drivers/StdPeriph/stm32f10x_rcc.c
)

target_include_directories(firmware PRIVATE
    include
    drivers/CMSIS
    drivers/StdPeriph
)

target_compile_definitions(firmware PRIVATE
    STM32F10X_MD
    USE_STDPERIPH_DRIVER
)

target_link_options(firmware PRIVATE
    -nostartfiles
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/firmware.map
    -Wl,--print-memory-usage
    -T${CMAKE_SOURCE_DIR}/boards/stm32f103c8t6/STM32F103C8Tx_FLASH.ld
)

target_link_libraries(firmware PRIVATE gcc)

add_custom_command(TARGET firmware POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:firmware> ${CMAKE_BINARY_DIR}/firmware.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware> ${CMAKE_BINARY_DIR}/firmware.bin
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:firmware>
    COMMENT "Generating HEX/BIN and printing firmware size"
)

add_custom_target(flash
    COMMAND st-flash --reset write ${CMAKE_BINARY_DIR}/firmware.bin 0x08000000
    DEPENDS firmware
    COMMENT "Flashing firmware with ST-Link"
)
