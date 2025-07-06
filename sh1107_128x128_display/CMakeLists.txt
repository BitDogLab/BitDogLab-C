# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Display_128x128_teste C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(lib/u8g2)

# Add executable. Default name is the project name, version 0.1

add_executable(Display_128x128_teste Display_128x128_teste.c inc/u8x8_gpio_and_delay_pico.c)

pico_set_program_name(Display_128x128_teste "Display_128x128_teste")
pico_set_program_version(Display_128x128_teste "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Display_128x128_teste 0)
pico_enable_stdio_usb(Display_128x128_teste 1)

# Add the standard library to the build
target_link_libraries(Display_128x128_teste
        pico_stdlib
        hardware_i2c
        u8g2)

# Add the standard include files to the build
target_include_directories(Display_128x128_teste PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Incluir a pasta com os headers da u8g2
target_include_directories(${PROJECT_NAME} PRIVATE u8g2_min)

pico_add_extra_outputs(Display_128x128_teste)

