#Change this for each assignment!
set(APPNAME Blunder)

#additional source files will have to be added here
add_executable(${APPNAME} main.cpp    )

#Link + include dependencies
target_link_libraries(${APPNAME} PUBLIC core IMGUI glm)
target_include_directories(${APPNAME} PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Copies this assignment1's asset folder to bin when it is built
add_custom_target(copyAssets${APPNAME} ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

#Trigger asset copy when assignment1 is built
add_dependencies(${APPNAME} copyAssets${APPNAME})