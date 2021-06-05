#Build file for UB

cmake_minimum_required (VERSION 3.10)


set (SRC_FILES
    "Main.cpp"
    "UB.hpp"
    "UB.cpp"
    "WorldShader.hpp"
    "World.hpp"
    "World.cpp"
    "Scripting/ScriptManager.hpp"
    "Scripting/ScriptManager.cpp"
    "Scripting/ScriptAction.hpp"
    "Scripting/ScriptAction.cpp"
    "Scripting/StartAction.hpp"
    "Scripting/StartAction.cpp"
    "Scripting/EntMoveAction.hpp"
    "Scripting/EntMoveAction.cpp"
    "Scripting/EntTeleportAction.hpp"
    "Scripting/EntTeleportAction.cpp"
    "Scripting/SoundAction.hpp"
    "Scripting/SoundAction.cpp"
    "GameStates/MainMenu.hpp"
    "GameStates/MainMenu.cpp"
    "GameStates/Gameplay.hpp"
    "GameStates/Gameplay.cpp"
    "GameStates/Editor.hpp"
    "GameStates/Editor.cpp"
    "UI/TilePanel.hpp" 
    "UI/TilePanel.cpp"
    "UI/EditorTab.hpp" 
    "UI/StructurePanel.hpp" 
    "UI/StructurePanel.cpp" 
    "UI/ScriptPanel.hpp" 
    "UI/ScriptPanel.cpp"
    "UI/RightPanel.hpp" 
    "UI/EditorPanel.hpp"
    "Structure.cpp"
    "Structure.hpp"
    "Character.cpp"
    "Character.hpp" 
    "UI/DialogueBox.hpp" 
    "UI/DialogueBox.cpp" "Entity.hpp" "Entity.cpp" "Player.hpp" "Player.cpp" "Scripting/StartBattleAction.cpp" "Scripting/StartBattleAction.hpp" "Battle.hpp" "Battle.cpp" "Enemy.hpp" "Enemy.cpp" "UI/ProgressBar.hpp" "UI/BattleMenu.hpp" "UI/BattleMenu.cpp")
    

set (CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (UB ${SRC_FILES})

if (MSVC)
    target_compile_options(UB PUBLIC "/permissive-")
endif()


add_custom_command(TARGET UB PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/UB/Assets $<TARGET_FILE_DIR:UB>/Assets)

add_custom_command(TARGET UB PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/lib/Dlls $<TARGET_FILE_DIR:UB>)

# SDL libraries
set (SDL_LIBS SDL2 SDL2main SDL2_mixer SDL2_image SDL2_ttf SDL2_net)

# Steam libraries
set (STEAM_LIBS Steam/phonon Steam/openvr_api)

# link against CAGE, SDL and its extensions
target_link_libraries (UB PUBLIC CAGE ${SDL_LIBS})