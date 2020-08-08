# CMakeList.txt : CMake project for CAGE, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# All of the C++ files to compile

set(SRC_FILES 
	"CAGE.cpp"
	"CAGE.hpp"
	"Core/Game.cpp"
	"Core/Game.hpp"
	"Core/GameState.hpp"
	"Core/StateMachine.hpp"
	"Core/StateMachine.cpp"
	"Core/Window.cpp"
	"Core/Window.hpp"
	"Audio/AudioSource.hpp"
	"Audio/AudioSource.cpp"
	"Audio/Listener.hpp"
	"Audio/Listener.cpp"
	"Glad/glad.c"
	"Graphics/Camera/Camera.hpp"
	"Graphics/Camera/Camera.cpp"
	"Graphics/Vertex/Vertex.hpp"
	"Graphics/UI/UIElement.hpp"
	"Graphics/UI/UIElement.cpp"
	"Graphics/UI/LayoutGroup.hpp"
	"Graphics/UI/LayoutGroup.cpp"
	"Graphics/UI/Layout.hpp"
	"Graphics/UI/Layout.cpp"
	"Graphics/UI/Hoverable.hpp"
	"Graphics/UI/Button.hpp"
	"Graphics/UI/CheckBox.hpp"
	"Graphics/UI/CheckBox.cpp"
	"Graphics/UI/Text.hpp"
	"Graphics/UI/Text.cpp"
	"Graphics/UI/RadioButton.hpp"
	"Graphics/VertexLayouts/VertexAttribute.hpp"
	"Graphics/VertexArrays/VertexArray.hpp"
	"Graphics/Buffers/VertexBuffer.hpp"
	"Graphics/Buffers/FrameBuffer.hpp"
	"Graphics/Buffers/FrameBuffer.cpp"
	"Graphics/Textures/Texture.hpp"
	"Graphics/Textures/Texture.cpp"
	"Graphics/Uniforms/Uniforms.hpp"
	"Graphics/Uniforms/Uniforms.cpp"
	"Graphics/ShaderProgram/ShaderProgram.hpp"
	"Graphics/ShaderProgram/ShaderProgram.cpp"
	"Graphics/ShaderProgram/Generic3DShader.hpp"
	"Graphics/ShaderProgram/SpriteShader.hpp"
	"Graphics/Shaders/Shader.hpp"
	"Graphics/Shaders/Shader.cpp"
	"Graphics/Models/Mesh.hpp"
	"Graphics/Models/Model.hpp"
	"IO/Networking/Endpoint.hpp"
	"IO/Networking/Endpoint.cpp"
	"IO/MeshLoader.hpp"
	"IO/MeshLoader.cpp"
	"IO/Events/Event.hpp"
	"IO/Events/EventListener.hpp"
	"IO/Events/MouseMotionEvent.hpp"
	"IO/Events/ClickEvents.hpp"
	"IO/Events/KeyEvents.hpp"
	"IO/Events/EventDispatcher.hpp"
	"IO/InputManager.hpp"
	"IO/InputManager.cpp"
)

# Add source to this project's library.
add_library (CAGE STATIC ${SRC_FILES})

# TODO: Add tests and install targets if needed.
