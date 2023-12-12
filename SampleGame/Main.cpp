/*
	A sample project showing how to utilize CAGE
*/

#include <CAGE.hpp>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_ttf.h>
#include <Graphics/Context/Context.hpp>
#include <Graphics/Pipeline/PipelineBuilder.hpp>
#include <Graphics/Renderer/Renderer.hpp>
#include <Graphics/Buffers/VkVertexBuffer.hpp>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtx/io.hpp>
#include <IO/InputManager.hpp>
#include <Graphics/UI/Button.hpp>
#include <Graphics/UI/TextField.hpp>
#include <Graphics/UI/Debug/InputDebugger.hpp>
#include <GLM/glm/gtx/string_cast.hpp>
#include <format>
#include "SampleGame.hpp"

template <typename T>
using Ref = std::shared_ptr<T>;

auto main(int argc, char** argv) -> int
{
	cage::PlayGame<SampleGame>(argc, argv);
	return 0;
}