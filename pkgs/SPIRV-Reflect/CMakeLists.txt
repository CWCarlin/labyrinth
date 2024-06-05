add_library(spirv_reflect
	spirv_reflect.c
)

target_include_directories(spirv_reflect
	PUBLIC
	spirv_reflect.h
	include/spirv/unified1/spirv.h
)

target_link_libraries(${PROJECT_NAME} spirv_reflect)