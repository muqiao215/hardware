include_guard(GLOBAL)

function(stm32f103_append_module out_var board module_name)
    get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    set(BOARD_DIR "${PROJECT_ROOT}/boards/${board}")
    set(COMMON_DIR "${PROJECT_ROOT}/common")

    set(module_sources)

    if(module_name STREQUAL "board")
        list(APPEND module_sources
            "${BOARD_DIR}/board.c"
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
        )
    elseif(module_name STREQUAL "delay")
        list(APPEND module_sources
            "${COMMON_DIR}/utils/delay.c"
        )
    elseif(module_name STREQUAL "exti")
        list(APPEND module_sources
            "${COMMON_DIR}/stdperiph/stm32f10x_exti.c"
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
        )
    elseif(module_name STREQUAL "tim")
        list(APPEND module_sources
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_tim.c"
        )
    elseif(module_name STREQUAL "tim2_pwm")
        list(APPEND module_sources
            "${COMMON_DIR}/drivers/tim2_pwm.c"
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_tim.c"
        )
    elseif(module_name STREQUAL "adc")
        list(APPEND module_sources
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_adc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
        )
    elseif(module_name STREQUAL "dma")
        list(APPEND module_sources
            "${COMMON_DIR}/stdperiph/stm32f10x_dma.c"
        )
    elseif(module_name STREQUAL "usart")
        list(APPEND module_sources
            "${COMMON_DIR}/drivers/usart_cli_port.c"
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_usart.c"
        )
    elseif(module_name STREQUAL "cli")
        list(APPEND module_sources
            "${COMMON_DIR}/cli/cli_input_pump.c"
            "${COMMON_DIR}/utils/cli_parser.c"
            "${COMMON_DIR}/utils/cli_dispatch.c"
            "${COMMON_DIR}/utils/ringbuf.c"
        )
    elseif(module_name STREQUAL "ringbuf")
        list(APPEND module_sources
            "${COMMON_DIR}/utils/ringbuf.c"
        )
    elseif(module_name STREQUAL "debounce")
        list(APPEND module_sources
            "${COMMON_DIR}/utils/debounce.c"
        )
    elseif(module_name STREQUAL "i2c")
        list(APPEND module_sources
            "${COMMON_DIR}/drivers/i2c_bus.c"
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_i2c.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
        )
    elseif(module_name STREQUAL "spi")
        list(APPEND module_sources
            "${COMMON_DIR}/drivers/spi_bus.c"
            "${COMMON_DIR}/stdperiph/misc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_spi.c"
        )
    elseif(module_name STREQUAL "soft_i2c_bus")
        list(APPEND module_sources
            "${COMMON_DIR}/drivers/soft_i2c_bus.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_gpio.c"
            "${COMMON_DIR}/stdperiph/stm32f10x_rcc.c"
        )
    elseif(module_name STREQUAL "ssd1306")
        list(APPEND module_sources
            "${COMMON_DIR}/components/ssd1306.c"
        )
    elseif(module_name STREQUAL "framebuffer")
        list(APPEND module_sources
            "${COMMON_DIR}/utils/framebuffer.c"
        )
    else()
        message(FATAL_ERROR "Unknown STM32 module: ${module_name}")
    endif()

    set(${out_var} "${${out_var}};${module_sources}" PARENT_SCOPE)
endfunction()

function(add_stm32f103_example)
    set(options)
    set(oneValueArgs TARGET BOARD)
    set(multiValueArgs SOURCES MODULES INCLUDE_DIRS DEFINITIONS)
    cmake_parse_arguments(EX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT EX_TARGET)
        message(FATAL_ERROR "add_stm32f103_example requires TARGET")
    endif()

    if(NOT EX_BOARD)
        set(EX_BOARD stm32f103c8t6)
    endif()

    get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    set(BOARD_DIR "${PROJECT_ROOT}/boards/${EX_BOARD}")
    set(COMMON_DIR "${PROJECT_ROOT}/common")
    set(example_sources
        ${EX_SOURCES}
        "${COMMON_DIR}/cmsis/system_stm32f10x.c"
        "${COMMON_DIR}/core/interrupts.c"
        "${BOARD_DIR}/startup_stm32f103c8t6.s"
    )

    foreach(module_name IN LISTS EX_MODULES)
        stm32f103_append_module(example_sources "${EX_BOARD}" "${module_name}")
    endforeach()

    list(REMOVE_DUPLICATES example_sources)

    add_executable(${EX_TARGET} ${example_sources})

    target_include_directories(${EX_TARGET} PRIVATE
        "${COMMON_DIR}/cmsis"
        "${COMMON_DIR}/core"
        "${COMMON_DIR}/cli"
        "${COMMON_DIR}/drivers"
        "${COMMON_DIR}/components"
        "${COMMON_DIR}/utils"
        "${COMMON_DIR}/stdperiph"
        "${BOARD_DIR}"
        ${EX_INCLUDE_DIRS}
    )

    target_compile_definitions(${EX_TARGET} PRIVATE
        STM32F10X_MD
        USE_STDPERIPH_DRIVER
        ${EX_DEFINITIONS}
    )

    target_compile_options(${EX_TARGET} PRIVATE
        -Wall
        -Wextra
        -Werror
    )

    target_link_options(${EX_TARGET} PRIVATE
        -nostartfiles
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_BINARY_DIR}/${EX_TARGET}.map
        -Wl,--print-memory-usage
        -T${BOARD_DIR}/STM32F103C8Tx_FLASH.ld
    )

    target_link_libraries(${EX_TARGET} PRIVATE gcc)

    add_custom_command(TARGET ${EX_TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${EX_TARGET}> ${CMAKE_BINARY_DIR}/${EX_TARGET}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EX_TARGET}> ${CMAKE_BINARY_DIR}/${EX_TARGET}.bin
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${EX_TARGET}>
        COMMENT "Generating HEX/BIN and printing firmware size"
    )

    add_custom_target(flash
        COMMAND st-flash --reset write ${CMAKE_BINARY_DIR}/${EX_TARGET}.bin 0x08000000
        DEPENDS ${EX_TARGET}
        COMMENT "Flashing ${EX_TARGET} with ST-Link"
    )
endfunction()
