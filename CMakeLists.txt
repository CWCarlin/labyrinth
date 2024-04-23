cmake_minimum_required(VERSION 3.5)

project(labyrinth VERSION 0.0.1 LANGUAGES C)
set(CMAKE_C_STANDARD 99)
cmake_policy(SET CMP0079 NEW)

add_compile_options(-D LOGGING)
add_compile_options(-std=c99 -Wall -Wextra -Wpedantic -Wno-strict-prototypes -Wno-language-extension-token)

if(WIN32)
	add_executable(labyrinth main.c pkgs/vma/vma_win.o)
else()
	add_executable(labyrinth main.c pkgs/vma/vma_linux.o)
	target_link_libraries(${PROJECT_NAME} stdc++)
endif()


target_include_directories(${PROJECT_NAME}
PUBLIC
	pkgs
)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

add_subdirectory(core)
