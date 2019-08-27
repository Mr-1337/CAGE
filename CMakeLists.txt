# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

project ("CAGE")

include_directories ("include")
link_directories ("lib")

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)


# Include sub-projects.
add_subdirectory ("CAGE")
target_compile_features("CAGE" PRIVATE cxx_std_17)

add_subdirectory ("SampleGame")
target_compile_features("CAGE" PRIVATE cxx_std_17)