# sample game configuration

cmake_minimum_required (VERSION 3.10)

set(SRC_FILES
	"Main.cpp" 
	"Shrektris/Shrektris.cpp" 
	"Shrektris/Shrektris.hpp"
	"STR/STR.cpp"
	"STR/STR.hpp" 
	"STR/Car.cpp" 
	"STR/Car.hpp" 
	"STR/Perlin.cpp" 
	"STR/Perlin.hpp"

	"STR/Racers/Racer.hpp"
	"STR/Racers/Racer.cpp"
	"STR/Racers/Shrek.cpp"
	"STR/Racers/Shrek.hpp"

	"STR/Track/Track.hpp"
	"STR/Track/Track.cpp"

	"STR/GameStates/Sandbox.cpp"
	"STR/GameStates/Sandbox.hpp"
	"STR/GameStates/MainMenu.cpp"
	"STR/GameStates/MainMenu.hpp"
	"STR/GameStates/Lobby.cpp"
	"STR/GameStates/Lobby.hpp"
	"STR/GameStates/UITest.hpp"
	"STR/GameStates/UITest.cpp"
	"STR/GameStates/Editor.hpp"
	"STR/GameStates/Editor.cpp"

	"STR/MenuButton.hpp"
)

add_executable (SampleGame ${SRC_FILES})

install (DIRECTORY Assets/ DESTINATION ${CMAKE_BINARY_DIR}/Assets/)

add_custom_command(TARGET SampleGame PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/SampleGame/Assets $<TARGET_FILE_DIR:SampleGame>/Assets)

add_custom_command(TARGET SampleGame PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/Dlls $<TARGET_FILE_DIR:SampleGame>)

# SDL libraries
set (SDL_LIBS SDL2/SDL2 SDL2/SDL2main SDL2/SDL2_mixer SDL2/SDL2_image SDL2/SDL2_ttf SDL2/SDL2_net)

# Steam libraries

set (STEAM_LIBS Steam/phonon Steam/openvr_api)

# link against CAGE, SDL and its extensions
target_link_libraries (SampleGame PUBLIC CAGE ${SDL_LIBS} ${STEAM_LIBS})