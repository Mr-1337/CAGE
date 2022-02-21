# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)

project ("CAGE"
		VERSION 0.1
		DESCRIPTION "Considerably Average Game Engine"
		LANGUAGES C CXX
		)

include_directories ("include" "CAGE")
link_directories ("lib" "lib/SDL2" "lib/Steam" "lib/assimp" "lib/vulkan")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include sub-projects.
add_subdirectory ("CAGE")

add_subdirectory ("SampleGame")

add_subdirectory ("STR")

add_subdirectory ("UB")

add_subdirectory ("V2")
