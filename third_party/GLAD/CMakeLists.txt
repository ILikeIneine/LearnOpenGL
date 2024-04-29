# GLAD

add_library(GLAD STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)
target_include_directories(GLAD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)