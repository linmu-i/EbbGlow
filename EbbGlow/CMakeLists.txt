cmake_minimum_required (VERSION 3.12)

add_compile_options("/utf-8")

project(EbbGlow VERSION 0.0.0)
add_library(EbbGlow STATIC)

target_compile_features(EbbGlow PRIVATE cxx_std_20)

target_include_directories(EbbGlow PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(RAYLIB_INCLUDE_DIR "$ENV{RAYLIB_DIR}/include")
set(RAYLIB_LIB_DIR "$ENV{RAYLIB_DIR}/lib")

target_include_directories(EbbGlow PUBLIC "${RAYLIB_INCLUDE_DIR}")
target_link_directories(EbbGlow PUBLIC "${RAYLIB_LIB_DIR}")
target_link_libraries(EbbGlow PUBLIC raylib)

if (WIN32)
	target_link_libraries(EbbGlow PUBLIC "winmm.lib")
endif()

set(CORE_SOURCES
	"src/EbbGlow/Core/ECS/IdManager.cpp"
	"src/EbbGlow/Core/Message/MessageManager.cpp"
	"src/EbbGlow/Core/World/World2D.cpp"
)

set(GRAPHICS_SOURCES
	"src/EbbGlow/Graphics/Graphics.cpp"
)

set(UTILS_SOURCES
	"src/EbbGlow/Utils/Control.cpp"
	"src/EbbGlow/Utils/Functions.cpp"
	"src/EbbGlow/Utils/Input_Raylib_Win32.cpp"
	"src/EbbGlow/Utils/Resource.cpp"
	"src/EbbGlow/Utils/ResourceCreator.cpp"
	"src/EbbGlow/Utils/ThreadPool.cpp"
)

set(UI_YUI_SOURCES
	"src/EbbGlow/UI/YUI/Control.cpp"
	"src/EbbGlow/UI/YUI/Transform.cpp"
	"src/EbbGlow/UI/YUI/View.cpp"
	"src/EbbGlow/UI/YUI/YUI.cpp"
	"src/EbbGlow/UI/YUI/ImageBox/ImageBox.cpp"
	"src/EbbGlow/UI/YUI/Button/ButtonLogic.cpp"
	"src/EbbGlow/UI/YUI/Button/ButtonCallback.cpp"
	"src/EbbGlow/UI/YUI/Button/FillButtonVisual.cpp"
	"src/EbbGlow/UI/YUI/Button/InvertButtonVisual.cpp"
	"src/EbbGlow/UI/YUI/Button/ImageButtonVisual.cpp"
	"src/EbbGlow/UI/YUI/TextBox/TextBox.cpp"
	"src/EbbGlow/UI/YUI/ScrollContainer/ScrollContainer.cpp"
	"src/EbbGlow/UI/YUI/UILayout/ContextStack.cpp"
	"src/EbbGlow/UI/YUI/UILayout/LayoutCom.cpp"
	"src/EbbGlow/UI/YUI/Click/ClickDispatch.cpp"
 "src/EbbGlow/UI/YUI/Button/ButtonMessage.cpp")

set(COMPONENTS_SOURCES
	"src/EbbGlow/Components/Children.cpp"
	"src/EbbGlow/Components/MessageCallback.cpp"
)

#[[
set(VISUALNOVEL_SOURCES
	"src/EbbGlow/VisualNovel/ScriptLoader/GlobalFunctions.cpp"
	"src/EbbGlow/VisualNovel/ScriptLoader/MusicManager.cpp"
	"src/EbbGlow/VisualNovel/ScriptLoader/SaveLoad.cpp"
	"src/EbbGlow/VisualNovel/ScriptLoader/SceneFunctions.cpp"
	"src/EbbGlow/VisualNovel/ScriptLoader/SceneTypes.cpp"
	"src/EbbGlow/VisualNovel/ScriptLoader/ScriptData.cpp"
	"src/EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.cpp"
	"src/EbbGlow/VisualNovel/UI/BackLog.cpp"
	"src/EbbGlow/VisualNovel/UI/SLControl.cpp"
	"src/EbbGlow/VisualNovel/UI/UIState.cpp"
	"src/EbbGlow/VisualNovel/VisualNovel/ButtonAttachment.cpp"
	"src/EbbGlow/VisualNovel/VisualNovel/ColorTween.cpp"
	"src/EbbGlow/VisualNovel/VisualNovel/DrawToScreen.cpp"
	"src/EbbGlow/VisualNovel/VisualNovel/MainTextBox.cpp"
	"src/EbbGlow/VisualNovel/VisualNovel/VisualNovel.cpp"
)]]

target_sources(EbbGlow PRIVATE
	${CORE_SOURCES}
	${GRAPHICS_SOURCES}
	${UTILS_SOURCES}
	${UI_YUI_SOURCES}
	${COMPONENTS_SOURCES}
	#${VISUALNOVEL_SOURCES}
 )

target_include_directories(EbbGlow PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(EbbGlow PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/include")