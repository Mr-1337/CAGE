# CMakeList.txt : CMake project for CAGE, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# All of the C++ files to compile

set(SRC_FILES 
	"CAGE.cpp"
	"CAGE.hpp"
	"Glad/glad.c"
	"Graphics/Uniforms/Uniforms.cpp"
	"Graphics/ShaderProgram/ShaderProgram.cpp"
	"Graphics/Shaders/Shader.cpp"
	"IO/MeshLoader.cpp"
)

# Add source to this project's library.
add_library (CAGE STATIC ${SRC_FILES})

# TODO: Add tests and install targets if needed.
