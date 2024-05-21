cmake_minimum_required (VERSION 3.8)

project (Common)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADER
				${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
				${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_HEADER})

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
