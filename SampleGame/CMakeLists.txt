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
	"Shrektris/Lobby.cpp")

add_executable (SampleGame ${SRC_FILES})

add_custom_command(TARGET SampleGame PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/SampleGame/Assets $<TARGET_FILE_DIR:SampleGame>/Assets)

add_custom_command(TARGET SampleGame PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/Dlls $<TARGET_FILE_DIR:SampleGame>)

# SDL libraries
set (SDL_LIBS SDL2/SDL2 SDL2/SDL2main SDL2/SDL2_mixer SDL2/SDL2_image SDL2/SDL2_ttf SDL2/SDL2_net)

# Steam libraries

set (STEAM_LIBS Steam/openvr_api)

# link against CAGE, SDL and its extensions
target_link_libraries (SampleGame PUBLIC CAGE ${SDL_LIBS} ${STEAM_LIBS})