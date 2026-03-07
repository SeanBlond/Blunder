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

add_library(core STATIC ${CORE_SRC} ${CORE_INC}    "shader/shader.h" "shader/shader.cpp"
												   "object/object.h" "object/object.cpp" "object/mesh.h" "object/mesh.cpp" 
												   "math/smath.h"  
												   "Blunder/Runtime/StateMachine.h" "Blunder/Runtime/StateMachine.cpp" "Blunder/Runtime/WindowManager.h" "Blunder/Runtime/WindowManager.cpp"
												   "line/line.h" "line/line.cpp" 
												   "Camera/camera.h" "Camera/camera.cpp" 
												   "Blunder/UI/UIElements.h"  "Blunder/UI/UIWindows.h" "Blunder/UI/UIRenderer.h" "Blunder/UI/UIRenderer.cpp" "Blunder/UI/TextInput.h" "Blunder/UI/TextInput.cpp" "Blunder/Time.h" "Blunder/Color.h" "object/transform.h" "shader/material.h" "shader/material.cpp" 
												   "Blunder/UI/Windows/AttributeWindow.cpp" "Blunder/UI/Windows/HierarchyWindow.cpp" "Blunder/UI/Windows/ViewportWindow.cpp" "Blunder/UI/Windows/ColorWindow.cpp"
												   "Blunder/Keystroke.h" 
												   "Font/font.h" "Font/font.cpp" 
												   "Blunder/Scene/Scene.h" "Blunder/Scene/Scene.cpp"  "Blunder/Scene/HierarchyElement.h" "Blunder/Scene/HierarchyElement.cpp" "Blunder/UI/UIPositioning.h" "Blunder/UI/Elements/FloatEntry.cpp" "Blunder/UI/Elements/FloatSlider.cpp" "Blunder/UI/Elements/IntEntry.cpp" "Blunder/UI/Elements/IntSlider.cpp" "Blunder/UI/Elements/Toggle.cpp" "Blunder/UI/Elements/TextEntry.cpp" "Blunder/UI/Elements/Dropdown.cpp" "Blunder/UI/Elements/HierarchyTextEntry.cpp" "Blunder/UI/Elements/ViewNav.cpp" "Blunder/UI/Elements/ColorSelector.cpp" "Blunder/UI/Elements/AttributeHeader.cpp")

find_package(OpenGL REQUIRED)

target_link_libraries(core PUBLIC IMGUI glm)

install (TARGETS core DESTINATION lib)
install (FILES ${CORE_INC} DESTINATION include/core)

