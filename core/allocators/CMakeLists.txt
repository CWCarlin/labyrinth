add_library(allocators
	src/block_alloc.c
	src/linear_alloc.c
)

target_include_directories(allocators
PUBLIC
	include
PRIVATE
	src
)

target_link_libraries(allocators 
	utility
)
target_link_libraries(${PROJECT_NAME} allocators)