add_library(renderer
	src/core.c
	src/gpu_allocator.c
	src/shader.c
	src/swapchain.c
	src/window.c
)

target_include_directories(renderer
	PUBLIC
	include
	${Vulkan_INCLUDE_DIRS}
	PRIVATE
	src
)

target_link_libraries(renderer
	utils
	data_structures
)

target_link_libraries(${PROJECT_NAME} renderer ${CMAKE_SOURCE_DIR}/bin/vma.o stdc++)