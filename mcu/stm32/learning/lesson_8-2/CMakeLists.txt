cmake_minimum_required(VERSION 3.20)

project(embedded_firmware C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Please specify toolchain file")
endif()

set(BOARD "" CACHE STRING "Target board")
if(NOT BOARD)
    message(FATAL_ERROR "Please specify board: -DBOARD=stm32f103c8t6")
endif()

include(cmake/boards/${BOARD}.cmake)

set(APP "" CACHE STRING "Application to build")

file(GLOB APP_MAIN_FILES "${CMAKE_SOURCE_DIR}/src/main_*.c")
set(AVAILABLE_APPS "")
foreach(APP_FILE ${APP_MAIN_FILES})
    get_filename_component(APP_NAME ${APP_FILE} NAME_WE)
    string(REPLACE "main_" "" APP_NAME ${APP_NAME})
    list(APPEND AVAILABLE_APPS ${APP_NAME})
endforeach()

if(NOT APP)
    string(REPLACE ";" "\n    - " APP_LIST "${AVAILABLE_APPS}")
    message(FATAL_ERROR "Specify APP:\n  -DAPP=<app_name>\nAvailable:\n    - ${APP_LIST}")
endif()

if(APP STREQUAL "default")
    set(APP_MAIN_SRC "${CMAKE_SOURCE_DIR}/src/main.c")
else()
    set(APP_MAIN_SRC "${CMAKE_SOURCE_DIR}/src/main_${APP}.c")
endif()

if(NOT EXISTS ${APP_MAIN_SRC})
    message(FATAL_ERROR "App '${APP}' not found: ${APP_MAIN_SRC}")
endif()

# Common sources for lesson_8-2 (DMA + AD Multi-Channel)
set(COMMON_SOURCES
    src/Delay.c
    src/OLED.c
    src/AD.c
)

add_executable(firmware.elf
    ${APP_MAIN_SRC}
    ${COMMON_SOURCES}
    ${STARTUP}
)

target_link_libraries(firmware.elf PRIVATE cmsis hal)
target_include_directories(firmware.elf PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(firmware.elf PRIVATE ${BOARD_DEFINES})
target_compile_options(firmware.elf PRIVATE ${BOARD_C_FLAGS})
target_link_options(firmware.elf PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_BINARY_DIR}/firmware.map
    -Wl,--print-memory-usage
    ${BOARD_LINK_FLAGS}
)

add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware.elf> ${CMAKE_BINARY_DIR}/firmware.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:firmware.elf> ${CMAKE_BINARY_DIR}/firmware.hex
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:firmware.elf>
)

message(STATUS "Done")