add_executable(Texture load_texture.cpp)
target_link_libraries(Texture glfw GLAD)

set_target_properties(
             Texture
             PROPERTIES
             WIN32_EXECUTABLE 
             TRUE
)