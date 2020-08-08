#Build file for UB

cmake_minimum_required (VERSION 3.10)

set (SRC_FILES
    "Main.cpp"
    "UB.hpp"
    "UB.cpp"
)

add_executable (UB ${SRC_FILES})

# SDL libraries
set (SDL_LIBS SDL2/SDL2 SDL2/SDL2main SDL2/SDL2_mixer SDL2/SDL2_image SDL2/SDL2_ttf SDL2/SDL2_net)

# Steam libraries
set (STEAM_LIBS Steam/phonon Steam/openvr_api)

# link against CAGE, SDL and its extensions
target_link_libraries (UB PUBLIC CAGE ${SDL_LIBS})