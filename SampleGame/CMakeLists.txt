# sample game configuration

cmake_minimum_required (VERSION 3.8)

add_executable (SampleGame Main.cpp)

# SDL libraries
set (SDL_LIBS SDL2/SDL2 SDL2/SDL2main)

# link against CAGE, SDL and its extensions
target_link_libraries (SampleGame PUBLIC CAGE ${SDL_LIBS})