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

project(picow_ble_temp_reader C CXX ASM)

set(WIFI_SSID "Your Wi-Fi SSID")
set(WIFI_PASSWORD "Your Wi-Fi Password")

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# Standalone example that reads from the on board temperature sensor and sends notifications via BLE
# Flashes slowly each second to show it's running
add_executable(picow_ble_temp_sensor
    server.c server_common.c
    )

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(picow_ble_temp_sensor 0)
pico_enable_stdio_usb(picow_ble_temp_sensor 1)

target_link_libraries(picow_ble_temp_sensor
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    hardware_adc
    )
target_include_directories(picow_ble_temp_sensor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} # For btstack config
    )
pico_btstack_make_gatt_header(picow_ble_temp_sensor PRIVATE "${CMAKE_CURRENT_LIST_DIR}/temp_sensor.gatt")

pico_add_extra_outputs(picow_ble_temp_sensor)

# Standalone example that connects to picow_ble_temp_sensor and reads the temperature 
# Flahes once quickly each second when it's running but not connected to another device
# Flashes twice quickly each second when connected to another device and reading it's temperature
add_executable(picow_ble_temp_reader
    client.c
    )
target_link_libraries(picow_ble_temp_reader
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    hardware_adc
    )
target_include_directories(picow_ble_temp_reader PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} # For btstack config
    )
target_compile_definitions(picow_ble_temp_reader PRIVATE
    RUNNING_AS_CLIENT=1
)

pico_add_extra_outputs(picow_ble_temp_reader)

if (WIFI_SSID AND WIFI_PASSWORD)
    # Another version of the sensor example, but this time also runs iperf over wifi
    add_executable(picow_ble_temp_sensor_with_wifi
        server_with_wifi.c server_common.c
        )
    target_link_libraries(picow_ble_temp_sensor_with_wifi
        pico_stdlib
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_iperf
        hardware_adc
        )
    target_include_directories(picow_ble_temp_sensor_with_wifi PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} # For btstack config
        )
    target_compile_definitions(picow_ble_temp_sensor_with_wifi PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
    pico_btstack_make_gatt_header(picow_ble_temp_sensor_with_wifi PRIVATE "${CMAKE_CURRENT_LIST_DIR}/temp_sensor.gatt")

    pico_add_extra_outputs(picow_ble_temp_sensor_with_wifi)
endif()
