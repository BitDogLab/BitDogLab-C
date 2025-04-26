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
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(unit_test_temp C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adicione diretórios necessários
add_subdirectory(lib/unity)

# Add executable. Default name is the project name, version 0.1

add_executable(unit_test_temp unit_test_temp.c )

pico_set_program_name(unit_test_temp "unit_test_temp")
pico_set_program_version(unit_test_temp "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(unit_test_temp 0)
pico_enable_stdio_usb(unit_test_temp 1)

# Add the standard library to the build
target_link_libraries(unit_test_temp
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(unit_test_temp PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        lib/unity)

# Add any user requested libraries
target_link_libraries(unit_test_temp 
        unity::framework
        )

pico_add_extra_outputs(unit_test_temp)

