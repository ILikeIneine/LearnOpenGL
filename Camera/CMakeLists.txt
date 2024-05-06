add_executable(Camera camera.cpp)

target_include_directories(Camera PRIVATE ${CMAKE_SOURCE_DIR}/third_party)

target_link_libraries(Camera glfw GLAD glm)

set_target_properties(
             Camera
             PROPERTIES
             WIN32_EXECUTABLE 
             TRUE
)