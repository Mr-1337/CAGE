# The sample project for a CAGE game

cmake_minimum_required (VERSION 3.20)

set(SRC_FILES "Main.cpp" "SampleGame.hpp" "SampleGame.cpp")

add_executable (SampleGame ${SRC_FILES})

find_program(GLSLC glslc)

set (SHADERS 
	"vktest.vert"
	"vktest.frag"
	"generic3D.vert"
	"generic3D.frag"
	"ui.vert"
	"ui.frag"
)

set (SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders")
list (TRANSFORM SHADERS PREPEND "${SHADER_DIR}/")

foreach(SHADER ${SHADERS})
	set (COMPILED "${SHADER}.spv")
	add_custom_command(
		OUTPUT "${COMPILED}"
		COMMENT "$Compiling shader ${SHADER}"
		COMMAND "${GLSLC}" "${SHADER}" -o "${COMPILED}"
		DEPENDS ${SHADER}
	)
	message ("Compiling shader ${SHADER}")
endforeach()

list (TRANSFORM SHADERS APPEND ".spv" OUTPUT_VARIABLE COMPILED)
add_custom_target (ShaderCompilation ALL DEPENDS ${COMPILED})


add_custom_command(TARGET ShaderCompilation PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:SampleGame>/Assets
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
)

add_custom_command(TARGET SampleGame PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/lib/Dlls $<TARGET_FILE_DIR:SampleGame>)

add_dependencies(SampleGame ShaderCompilation)

# link against CAGE, SDL and its extensions
target_link_libraries (SampleGame PUBLIC CAGE)