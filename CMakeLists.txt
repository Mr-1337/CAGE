# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

project ("CAGE"
		VERSION 0.0.1
		DESCRIPTION "Considerably Average Game Engine"
		LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include sub-projects.

add_subdirectory ("Glad")

add_subdirectory ("CAGE")

target_include_directories (CAGE PUBLIC "include" "CAGE" "${PROJECT_BINARY_DIR}")
target_link_libraries(CAGE PUBLIC GLAD)

get_property(dirs TARGET CAGE PROPERTY INCLUDE_DIRECTORIES)
message(STATUS "${dirs}")
list (GET dirs 0 dir)
message(STATUS "dir=${dir}")


# SDL libraries
set (SDL_LIBS SDL2 SDL2main SDL2_mixer SDL2_image SDL2_ttf SDL2_net)

# Steam libraries
set (STEAM_LIBS phonon openvr_api)

# Vulkan libraries
set (VULKAN_LIBS vulkan-1)

target_precompile_headers(CAGE PUBLIC "${dir}/vulkan/vulkan.hpp")
target_link_directories (CAGE INTERFACE "lib" "lib/SDL2" "lib/Steam" "lib/assimp" "lib/vulkan")
target_link_libraries(CAGE INTERFACE ${SDL_LIBS} ${VULKAN_LIBS} assimp-vc142-mt)

configure_file("CAGE/Version.hpp.in" Version.hpp)

add_subdirectory ("SampleGame")

#add_subdirectory ("Shrektris")
#add_subdirectory ("STR")
#add_subdirectory ("UB")
#add_subdirectory ("V2")