add_library(math_lib
	src/prime.c
	src/quaternion.c
	src/vector.c
)

target_include_directories(math_lib
	PUBLIC
	include
	PRIVATE
	src
)

target_link_libraries(math_lib utils -lm)
target_link_libraries(${PROJECT_NAME} math_lib)