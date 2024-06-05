cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Labyrinth LANGUAGES C)
set(CMAKE_C_STANDARD 99)
cmake_policy(SET CMP0079 NEW)

add_compile_options(-D LOGGING -D_GNU_SOURCE)
add_compile_options(-std=c99 -Werror -Wall -Wformat -Wextra -Wpedantic -Wno-strict-prototypes
	-Wno-language-extension-token -Wno-empty-translation-unit)

add_executable(Labyrinth main.c)

find_package(Vulkan REQUIRED COMPONENTS glslc)

add_subdirectory(labyrinth)
add_subdirectory(pkgs)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

file(GLOB_RECURSE GLSL_SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/assets/shaders/glsl/vertex/*.vert"
	"${PROJECT_SOURCE_DIR}/assets/shaders/glsl/fragment/*.frag"
	"${PROJECT_SOURCE_DIR}/assets/shaders/glsl/compute/*.comp"
)

set(SPIRV_BINARY_FILES)

foreach(GLSL ${GLSL_SOURCE_FILES})
	get_filename_component(FILE_NAME ${GLSL} NAME)
	set(SPIRV "${PROJECT_SOURCE_DIR}/assets/shaders/spv/${FILE_NAME}.spv")

	add_custom_command(
		OUTPUT ${SPIRV}
		COMMAND ${glslc_executable} ${GLSL} -o ${SPIRV}
		DEPENDS ${GLSL}
		COMMENT "Compiling ${GLSL}"
	)

	list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(
	Shaders ALL
	DEPENDS ${SPIRV_BINARY_FILES}
	COMMENT "Building all shaders"
)
