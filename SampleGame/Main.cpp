/*
 *	Welcome to the CAGE
 *
 *	Include the header for your game and make it the template argument for PlayGame<T>
 *
 */

#include <iostream>
#include "Shrektris/Shrektris.hpp"

#include <GLM/glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <SDL2/SDL_vulkan.h>

int main(int argc, char** argv)
{
	//cage::PlayGame<Shrektris>(argc, argv);

	SDL_Init(SDL_INIT_EVERYTHING);

	auto window = SDL_CreateWindow("Vulkan Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

	unsigned int count = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	std::vector<const char*> extensions(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());

	vk::ApplicationInfo appInfo("Shrektris", 1, "CAGE", 1, 1);
	vk::InstanceCreateInfo info({}, &appInfo);
	vk::Instance instance = vk::createInstance(info);

	vk::PhysicalDevice gpu = instance.enumeratePhysicalDevices().front();

	for (auto ext : extensions)
	{
		std::cout << ext << std::endl;
	}

	std::cout << count << std::endl;

	SDL_Delay(5000);

	std::cout << "It's not ogre, it's never ogre" << std::endl;
	return 0;
}
