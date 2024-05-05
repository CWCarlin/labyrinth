add_library(utils
	src/logging.c
	src/types.c
)

target_include_directories(utils
	PUBLIC
	include
	${Vulkan_INCLUDE_DIRS}
	PRIVATE
	src
)

target_link_libraries(${PROJECT_NAME} utils)