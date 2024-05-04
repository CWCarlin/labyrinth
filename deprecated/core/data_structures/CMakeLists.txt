add_library(data_structures
	src/list.c
	src/map.c
	src/queue.c
	src/set.c
	src/stack.c
)

target_include_directories(data_structures
PUBLIC
	include
PRIVATE
	src
)

target_link_libraries(data_structures
	utility
	allocators
)
target_link_libraries(${PROJECT_NAME} data_structures)