cmake_minimum_required(VERSION 3.13)

project(pico_bootsel_button C ASM)

add_library(pico_bootsel_button INTERFACE)

target_sources(pico_bootsel_button INTERFACE
	src/pico_bootsel_button.c
)

target_include_directories(pico_bootsel_button INTERFACE
	src
)