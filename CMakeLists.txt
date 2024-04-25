cmake_minimum_required (VERSION 3.8)

project ("LearnOpenGL")

#--------------------------------------------------------------------
# Hide the console window in visual studio projects
#--------------------------------------------------------------------
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

# library
include_directories( GLFW/include )
include_directories( GLAD/include )

add_library(GLFW STATIC IMPORTED)
set_target_properties(GLFW PROPERTIES IMPORTED_LOCATION "C:/Users/m01016/Git/glfw/out/build/x64-debug/src/glfw3.lib")

# sudirectories
add_subdirectory(HelloTriangle)
add_subdirectory(GLAD)

