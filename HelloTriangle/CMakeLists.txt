add_executable(HelloTrangle HelloTriangle.cpp)

target_link_libraries(HelloTrangle glfw GLAD glm-header-only)
target_include_directories(HelloTrangle PRIVATE ${CMAKE_SOURCE_DIR}/third_party)

set_target_properties(
             HelloTrangle
             PROPERTIES
             WIN32_EXECUTABLE 
             TRUE
)

