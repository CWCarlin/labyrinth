add_library(utils
	src/logging.c
)

target_include_directories(utils
PUBLIC
	include
PRIVATE
	src
)

target_link_libraries(utils Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} utils)