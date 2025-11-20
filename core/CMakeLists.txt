file(
 GLOB_RECURSE CORE_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

set(CORE_INC_DIR
 ${CMAKE_CURRENT_SOURCE_DIR}
 CACHE PATH "CORE INCLUDE SOURCE PATH"
)

add_library(core STATIC ${CORE_SRC} ${CORE_INC}    "shader/shader.h" "shader/shader.cpp" "object/object.h" "object/object.cpp" "object/mesh.h" "object/mesh.cpp" "math/smath.h"  "Blunder/StateMachine/StateMachine.h" "Blunder/StateMachine/StateMachine.cpp" "line/line.h" "line/line.cpp" "Camera/camera.h" "Camera/camera.cpp" "Blunder/UI/UIElements.h" "Blunder/Keystroke.h" "Font/font.h" "Font/font.cpp" "Blunder/UI/UIElements.cpp" "Blunder/UI/UIWindows.h" "Blunder/UI/UIRenderer.h" "Blunder/UI/UIRenderer.cpp" "Blunder/UI/TextInput.h" "Blunder/UI/TextInput.cpp" "Blunder/Time.h" "Blunder/Color.h" "object/transform.h" "Blunder/ObjectSystem/ObjectRender.h" "Blunder/ObjectSystem/ObjectRender.cpp" "shader/material.h" "shader/material.cpp" "Blunder/ObjectSystem/ObjectSystem.h" "Blunder/ObjectSystem/ObjectSystem.cpp" "Blunder/ObjectSystem/Elements/Folder.h" "Blunder/ObjectSystem/Elements/HierarchyElement.h" "Blunder/ObjectSystem/Elements/HierarchyElement.cpp" "Blunder/ObjectSystem/Elements/Folder.cpp")

find_package(OpenGL REQUIRED)

target_link_libraries(core PUBLIC IMGUI glm freetype)

install (TARGETS core DESTINATION lib)
install (FILES ${CORE_INC} DESTINATION include/core)

