cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Labyrinth LANGUAGES C)
set(CMAKE_C_STANDARD 99)
cmake_policy(SET CMP0079 NEW)

add_compile_options(-D LOGGING -D_GNU_SOURCE)
add_compile_options(-std=c99 -Werror -Wall -Wformat -Wextra -Wpedantic -Wno-strict-prototypes
	-Wno-language-extension-token -Wno-empty-translation-unit)

add_executable(Labyrinth main.c)

find_package(Vulkan REQUIRED)

add_subdirectory(labyrinth)