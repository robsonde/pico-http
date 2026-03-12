cmake_minimum_required(VERSION 4.2)

set(PICO_BOARD pico2_w)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(example C CXX)
pico_sdk_init()

add_executable(example
	network.c
        example.c
        )

target_include_directories(example PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..    # for our common lwipopts
        )

target_link_libraries(example
        pico_stdlib              # for core functionality
        pico_lwip_http           # for http functionality
	pico_cyw43_arch_lwip_threadsafe_background  # for Wifi hardware support
        )

	pico_enable_stdio_usb(example 1)     # turn on USB for serial output.
	pico_enable_stdio_uart(example 0)    # turn off uart for serial output.

# create map/bin/hex file etc.
pico_add_extra_outputs(example)



