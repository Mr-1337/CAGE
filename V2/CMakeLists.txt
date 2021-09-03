#Build file for UB

cmake_minimum_required (VERSION 3.10)


set (SRC_FILES
    "Main.cpp"
     "V2.cpp" 
     "V2.hpp"
     "Menu.cpp"
     "Menu.hpp"
     )
    

set (CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (V2 ${SRC_FILES})

if (MSVC)
    target_compile_options(V2 PUBLIC "/permissive-")
endif()


add_custom_command(TARGET V2 PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/UB/Assets $<TARGET_FILE_DIR:V2>/Assets)

add_custom_command(TARGET V2 PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/Dlls $<TARGET_FILE_DIR:V2>)

# SDL libraries
set (SDL_LIBS SDL2 SDL2main SDL2_mixer SDL2_image SDL2_ttf SDL2_net)

# Steam libraries
set (STEAM_LIBS phonon openvr_api)

# link against CAGE, SDL and its extensions
target_link_libraries (V2 PUBLIC CAGE ${SDL_LIBS})