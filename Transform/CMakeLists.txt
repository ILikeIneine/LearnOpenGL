add_executable(Transform transform.cpp "stb_image.h")

target_include_directories(Transform PRIVATE ${CMAKE_SOURCE_DIR}/third_party)

target_link_libraries(Transform glfw GLAD glm)

set_target_properties(
             Transform
             PROPERTIES
             WIN32_EXECUTABLE 
             FALSE
)