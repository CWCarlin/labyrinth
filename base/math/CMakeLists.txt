add_library(math
	src/vector.c
	src/quaternion.c
)

target_include_directories(math
	PUBLIC
	include
	PRIVATE
	src
)

target_link_libraries(math utils)
target_link_libraries(${PROJECT_NAME} math)