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
add_executable(finalProj ${FINALPROJ_SRC} ${FINALPROJ_INC} "notmain.h" "assets/outlines/outlines.cpp" "assets/outlines/outlines.h")
target_link_libraries(finalProj PUBLIC core IMGUI assimp)
target_include_directories(finalProj PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when finalProj is built
add_dependencies(finalProj copyAssetsFP)