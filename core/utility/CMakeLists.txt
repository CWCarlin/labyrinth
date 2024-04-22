add_library(utility
	src/logging.c
)

target_include_directories(utility
PUBLIC
	include
PRIVATE
	src
)

target_link_libraries(${PROJECT_NAME} utility)