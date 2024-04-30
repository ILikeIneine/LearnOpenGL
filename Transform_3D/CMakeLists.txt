add_executable(Transform_3D transform_3d.cpp)

target_include_directories(Transform_3D PRIVATE ${CMAKE_SOURCE_DIR}/third_party)

target_link_libraries(Transform_3D glfw GLAD glm)

set_target_properties(
             Transform_3D
             PROPERTIES
             WIN32_EXECUTABLE 
             TRUE
)