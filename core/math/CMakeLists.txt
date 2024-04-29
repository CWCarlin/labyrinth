add_library(math
    src/vector.c
)

target_include_directories(math
PUBLIC
    include
PRIVATE
    src
)

target_link_libraries(math
    -lm
    utility
)

target_link_libraries(${PROJECT_NAME} math)