add_library(fabric
	src/fabric.c
	src/fiber.c
	src/synchronize.c
	src/thread.c
)

target_include_directories(fabric
	PUBLIC
	include
	PRIVATE
	src
)

target_link_libraries(fabric
	utils
	allocators
	data_structures
)

target_link_libraries(${PROJECT_NAME} fabric)