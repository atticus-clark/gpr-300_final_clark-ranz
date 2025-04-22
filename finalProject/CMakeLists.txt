file(
 GLOB_RECURSE FINALPROJ_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE FINALPROJ_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this finalProj's asset folder to bin when it is built
add_custom_target(copyAssetsFP ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${FINALPROJ_INC} DESTINATION include/finalProj)
add_executable(finalProj ${FINALPROJ_SRC} ${FINALPROJ_INC} "notmain.h" "assets/program/object.h" "assets/program/light.h" "assets/program/compositeRenderer.h" "assets/program/compositeRenderer.cpp" "assets/program/regular.h" "assets/program/regular.cpp" "assets/program/shadows.h" "assets/program/shadows.cpp" "assets/program/outlines.h" "assets/program/outlines.cpp")
target_link_libraries(finalProj PUBLIC core IMGUI assimp)
target_include_directories(finalProj PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when finalProj is built
add_dependencies(finalProj copyAssetsFP)