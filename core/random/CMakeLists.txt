add_library(random
	src/random.c
)

target_include_directories(random
PUBLIC
	include
PRIVATE
	src
)

target_link_libraries(random
utils
)

target_link_libraries(${PROJECT_NAME} random)