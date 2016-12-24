# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (EAGL)
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)



include_directories(
	external/glfw-3.1.2/include/GLFW/
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
	SOIL
	assimp
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Tutorial 1
add_executable(main 
	eagl.cpp
	src/Shader.cpp
	shaders/default.vertexshader
	shaders/default.fragmentshader
)
target_link_libraries(main
	${ALL_LIBS}
)
# Xcode and Visual working directories
#set_target_properties(main PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
#create_target_launcher(main WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
