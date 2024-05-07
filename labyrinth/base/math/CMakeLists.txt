add_library(math
	src/prime.c
	src/quaternion.c
	src/vector.c
)

target_include_directories(math
	PUBLIC
	include
	PRIVATE
	src
)

target_link_libraries(math utils)
target_link_libraries(${PROJECT_NAME} math)