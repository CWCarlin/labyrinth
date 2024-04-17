add_library(data_structures
	src/list.c
	src/queue.c
	src/stack.c
)

target_include_directories(data_structures
PUBLIC
	include
PRIVATE
	src
)

target_link_libraries(data_structures
utils
)

target_link_libraries(${PROJECT_NAME} data_structures)