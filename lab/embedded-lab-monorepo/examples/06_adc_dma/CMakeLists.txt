cmake_minimum_required(VERSION 3.20)

project(example_adc_dma C ASM)

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Specify -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchains/arm-gcc.cmake")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/stm32f103_example.cmake)

add_stm32f103_example(
    TARGET firmware
    BOARD stm32f103c8t6
    SOURCES ${CMAKE_CURRENT_LIST_DIR}/main.c
    MODULES
        board
        delay
        adc
        dma
)
