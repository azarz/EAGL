# CMake entry point
cmake_minimum_required(VERSION 3.14)
project(EAGL)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EMSCRIPTEN)
	find_package(OpenGL REQUIRED)
endif()

# Compile external dependencies
add_subdirectory(external)



include_directories(
    external/glm-0.9.7.1/
    external/assimp-3.0.1270/include/
    include/
    .
)

# EAGL
add_executable(EAGL
	main.cpp
	src/Shader.cpp
	src/ImageLoader.cpp
	src/FrustumG.cpp
	src/Plane.cpp
	src/Vec3.cpp
	shaders/default.vertexshader
	shaders/default.fragmentshader
)
target_link_libraries(EAGL
	assimp
)

if(EMSCRIPTEN)
	file(GLOB_RECURSE EAGL_WEB_ASSETS CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/model/*"
		"${CMAKE_CURRENT_SOURCE_DIR}/texture/*"
		"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*"
	)
	set_property(TARGET EAGL APPEND PROPERTY LINK_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
		${EAGL_WEB_ASSETS}
	)
	include(FetchContent)
	FetchContent_Declare(stb_image
		GIT_REPOSITORY https://github.com/nothings/stb.git
		GIT_TAG a7d4c2175630e1ea5b6f8dd00ab96fed0b97b96d
		GIT_SHALLOW TRUE
	)
	FetchContent_MakeAvailable(stb_image)
	target_include_directories(EAGL PRIVATE ${stb_image_SOURCE_DIR})
	target_link_options(EAGL PRIVATE
		"-sUSE_GLFW=3"
		"-sMIN_WEBGL_VERSION=2"
		"-sMAX_WEBGL_VERSION=2"
		"-sALLOW_MEMORY_GROWTH=1"
		"-sFORCE_FILESYSTEM=1"
		"--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
		"--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/model@/model"
		"--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/texture@/texture"
		"--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders"
	)
	set_target_properties(EAGL PROPERTIES
		OUTPUT_NAME index
		SUFFIX ".html"
	)
else()
	include_directories(
		external/glfw-3.1.2/include/GLFW/
		external/glfw-3.1.2/include/
		external/glew-1.13.0/include/
	)
	target_compile_definitions(EAGL PRIVATE
		TW_STATIC
		TW_NO_LIB_PRAGMA
		TW_NO_DIRECT3D
		GLEW_STATIC
		_CRT_SECURE_NO_WARNINGS
	)
	target_link_libraries(EAGL
		${OPENGL_LIBRARY}
		glfw
		GLEW_1130
		SOIL
	)
endif()
# Xcode and Visual working directories
#set_target_properties(main PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
#create_target_launcher(main WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
