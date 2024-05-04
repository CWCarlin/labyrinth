add_library(utils
	src/logging.c
)

target_include_directories(utils
	PUBLIC
	include
	PRIVATE
	${Vulkan_INCLUDE_DIRS}
	src
)

target_link_libraries(${PROJECT_NAME} utils)