cmake_minimum_required (VERSION 3.8)

project ("LearnOpenGL")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--------------------------------------------------------------------
# Hide the console window in visual studio projects
#--------------------------------------------------------------------
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

# library
#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/GLFW/include )
#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/GLAD/include )

# sudirectories
add_subdirectory(third_party/GLAD)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)
add_subdirectory(HelloTriangle)
add_subdirectory(ShaderToy)
add_subdirectory(Texture)
add_subdirectory(Transform)

