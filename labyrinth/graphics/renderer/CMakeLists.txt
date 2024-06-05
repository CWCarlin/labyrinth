add_library(renderer
	src/allocator.c
	src/core.c
	src/descriptor.c
	src/frame.c
	src/image.c
	src/mesh.c
	src/shader.c
	src/swapchain.c
	src/window.c
)

target_include_directories(renderer
	PUBLIC
	include
	${Vulkan_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/pkgs
	PRIVATE
	src
)

target_link_libraries(renderer
	utils
	data_structures
	spirv_reflect
)

target_link_libraries(${PROJECT_NAME} renderer ${CMAKE_SOURCE_DIR}/bin/vma.o stdc++)