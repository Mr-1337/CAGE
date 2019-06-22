# sample game configuration

cmake_minimum_required (VERSION 3.8)

add_executable (SampleGame Main.cpp)

#install (DIRECTORY Assets/ DESTINATION ${CMAKE_BINARY_DIR}/Assets/)

add_custom_command(TARGET SampleGame PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/SampleGame/Assets $<TARGET_FILE_DIR:SampleGame>/Assets)

add_custom_command(TARGET SampleGame PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/Dlls $<TARGET_FILE_DIR:SampleGame>)

# SDL libraries
set (SDL_LIBS SDL2/SDL2 SDL2/SDL2main SDL2/SDL2_mixer SDL2/SDL2_image SDL2/SDL2_net)

# link against CAGE, SDL and its extensions
target_link_libraries (SampleGame PUBLIC CAGE ${SDL_LIBS})