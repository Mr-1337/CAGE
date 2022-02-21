# sample game configuration

cmake_minimum_required (VERSION 3.10)

set(SRC_FILES
	"Main.cpp" 
	"MenuButton.hpp"
	"Shrektris/Shrektris.cpp" 
	"Shrektris/Shrektris.hpp"
	"Shrektris/ShrekMenu.hpp" 
	"Shrektris/ShrekMenu.cpp" 
	"Shrektris/Gameplay.hpp" 
	"Shrektris/Gameplay.cpp"
	"Shrektris/Lobby.hpp" 
	"Shrektris/Lobby.cpp" 
	"Shrektris/Player.hpp" 
	"Shrektris/Server.hpp" 
	"Shrektris/Client.hpp" 
	"MenuButton.cpp" 
	"Shrektris/Server.cpp" 
	"Shrektris/Client.cpp" 
	"Perlin.hpp" 
	"Perlin.cpp")

set (CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (Shrektris ${SRC_FILES})

if (MSVC)
    target_compile_options(Shrektris PUBLIC "/permissive-")
endif()

add_custom_command(TARGET Shrektris PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/SampleGame/Assets $<TARGET_FILE_DIR:Shrektris>/Assets)

add_custom_command(TARGET Shrektris PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/Dlls $<TARGET_FILE_DIR:Shrektris>)

# SDL libraries
set (SDL_LIBS SDL2 SDL2main SDL2_mixer SDL2_image SDL2_ttf SDL2_net)

# Steam libraries
set (STEAM_LIBS phonon openvr_api)

# Vulkan libraries
set (VULKAN_LIBS vulkan-1)

# link against CAGE, SDL and its extensions
target_link_libraries (Shrektris PUBLIC CAGE ${SDL_LIBS} ${STEAM_LIBS} ${VULKAN_LIBS} assimp-vc142-mt)