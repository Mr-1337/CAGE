﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

project ("CAGE"
		VERSION 0.1
		DESCRIPTION "Considerably Average Game Engine"
		LANGUAGES C CXX
		)

include_directories ("include" "CAGE")
link_directories ("lib" "lib/SDL2" "lib/Steam")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include sub-projects.
add_subdirectory ("CAGE")
target_compile_features("CAGE" PRIVATE cxx_std_17)

add_subdirectory ("SampleGame")
target_compile_features("SampleGame" PRIVATE cxx_std_17)

add_subdirectory ("STR")
target_compile_features("STR" PRIVATE cxx_std_17)

add_subdirectory ("UB")
target_compile_features("UB" PRIVATE cxx_std_17)