add_executable(ShaderToy ShaderToy.cpp )

target_link_libraries(ShaderToy glfw GLAD)

set_target_properties(
             ShaderToy
             PROPERTIES
             WIN32_EXECUTABLE 
             TRUE
)