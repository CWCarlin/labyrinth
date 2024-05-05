add_library(allocators
	src/alloc_types.c
	src/block_alloc.c
	src/linear_alloc.c
)

target_include_directories(allocators
	PUBLIC
	include
	PRIVATE
	src
)

target_link_libraries(allocators utils)
target_link_libraries(${PROJECT_NAME} allocators)