# sample game configuration

cmake_minimum_required (VERSION 3.10)

set(SRC_FILES
	"Main.cpp" 
	"STR.cpp"
	"STR.hpp" 
	"Car.cpp" 
	"Car.hpp" 
	"Perlin.cpp" 
	"Perlin.hpp"

	"Racers/Racer.hpp"
	"Racers/Racer.cpp"
	"Racers/Shrek.cpp"
	"Racers/Shrek.hpp"

	"Track/Track.hpp"
	"Track/Track.cpp"

	"GameStates/Sandbox.cpp"
	"GameStates/Sandbox.hpp"
	"GameStates/MainMenu.cpp"
	"GameStates/MainMenu.hpp"
	"GameStates/Lobby.cpp"
	"GameStates/Lobby.hpp"
	"GameStates/UITest.hpp"
	"GameStates/UITest.cpp"
	"GameStates/Editor.hpp"
	"GameStates/Editor.cpp"

	"MenuButton.hpp")
	
add_executable (STR ${SRC_FILES})

add_custom_command(TARGET STR PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/STR/Assets $<TARGET_FILE_DIR:STR>/Assets)

add_custom_command(TARGET STR PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/DLLs $<TARGET_FILE_DIR:STR>)

# SDL libraries
set (SDL_LIBS SDL2 SDL2main SDL2_mixer SDL2_image SDL2_ttf SDL2_net)

# Steam libraries

set (STEAM_LIBS phonon)

# link against CAGE, SDL and its extensions
target_link_libraries (STR PUBLIC CAGE ${SDL_LIBS} ${STEAM_LIBS})