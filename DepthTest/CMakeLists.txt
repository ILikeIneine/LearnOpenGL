Project(DepthTest)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADER 
                ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE 
                ${CMAKE_CURRENT_SOURCE_DIR}/*.c
                ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE} ${${PROJECT_NAME}_HEADER})

# import <glm> headers only
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party)

target_link_libraries(${PROJECT_NAME} glfw GLAD glm::glm assimp::assimp Common)

set_target_properties(
             ${PROJECT_NAME}
             PROPERTIES
             WIN32_EXECUTABLE
             TRUE
)