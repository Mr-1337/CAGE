# CMakeList.txt : CMake project for CAGE, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# All of the C++ files to compile

set(SRC_FILES 
	"CAGE.cpp"
	"CAGE.hpp"
	"Core/Game.cpp"
	"Core/Game.hpp"
	"Core/Window.cpp"
	"Core/Window.hpp"
	"Glad/glad.c"
	"Graphics/Camera/Camera.hpp"
	"Graphics/Camera/Camera.cpp"
	"Graphics/Vertex/Vertex.hpp"
	"Graphics/VertexLayouts/VertexAttribute.hpp"
	"Graphics/VertexArrays/VertexArray.hpp"
	"Graphics/Buffers/VertexBuffer.hpp"
	"Graphics/Uniforms/Uniforms.hpp"
	"Graphics/Uniforms/Uniforms.cpp"
	"Graphics/ShaderProgram/ShaderProgram.hpp"
	"Graphics/ShaderProgram/ShaderProgram.cpp"
	"Graphics/Shaders/Shader.hpp"
	"Graphics/Shaders/Shader.cpp"
	"Graphics/Models/Mesh.hpp"
	"Graphics/Models/Mesh.cpp"
	"IO/Networking/Endpoint.hpp"
	"IO/Networking/Endpoint.cpp"
	"IO/MeshLoader.hpp"
	"IO/MeshLoader.cpp"
)

# Add source to this project's library.
add_library (CAGE STATIC ${SRC_FILES})

# TODO: Add tests and install targets if needed.
