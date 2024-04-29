add_executable(HelloTrangle HelloTriangle.cpp)

target_link_libraries(HelloTrangle glfw GLAD)

set_target_properties(
             HelloTrangle
             PROPERTIES
             WIN32_EXECUTABLE 
             TRUE
)

