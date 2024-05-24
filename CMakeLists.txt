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


# sudirectories
add_subdirectory(Common)

add_subdirectory(third_party/GLAD)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/ASSIMP)

add_subdirectory(HelloTriangle)
add_subdirectory(ShaderToy)
add_subdirectory(Texture)
add_subdirectory(Transform)
add_subdirectory(Transform_3D)
add_subdirectory(Camera)
add_subdirectory(Model)
add_subdirectory(DepthTest)
add_subdirectory(StencilTest)
add_subdirectory(StencilTest_Reflection)
add_subdirectory(Billboard)
add_subdirectory(Blending)
add_subdirectory(FaceCulling)
add_subdirectory(FrameBuffer)

add_subdirectory(Colors)
add_subdirectory(BasicLighting)
add_subdirectory(Material)
add_subdirectory(LightingMaps)
add_subdirectory(LightingCasters)
add_subdirectory(LightSpot)
add_subdirectory(MultipleLight)

add_subdirectory(Cubemaps)
add_subdirectory(Cubemaps_refract)

add_subdirectory(AdvancedGLSL)
add_subdirectory(GeometryShader)