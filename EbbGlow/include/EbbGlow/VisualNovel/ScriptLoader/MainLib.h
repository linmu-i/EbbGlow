#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/GlobalFunctions.h>
#include <EbbGlow/VisualNovel/ScriptLoader/SceneFunctions.h>
#include <EbbGlow/VisualNovel/ScriptLoader/SceneTypes.h>


namespace ebbglow::visualnovel
{
	inline void ImportMainLib(ScriptLoader& scLoader)
	{
		scLoader.registerGlobalFunction("String", Global_String);
		scLoader.registerGlobalFunction("Number", Global_Number);
		scLoader.registerGlobalFunction("Include", Global_Include);
		scLoader.registerGlobalFunction("BeginScene", Global_BeginScene);

		scLoader.registerSceneType("TextScene", SceneType_TextScene);
		scLoader.registerSceneType("SelectScene", SceneType_SelectScene);
		scLoader.registerSceneType("DelayScene", SceneType_DelayScene);

		scLoader.registerSceneFunction("TextScene", Scene_TextScene);
		scLoader.registerSceneFunction("Chr", Scene_Chr);
		scLoader.registerSceneFunction("Button", Scene_Button);
		scLoader.registerSceneFunction("ImageBox", Scene_ImageBox);
		scLoader.registerSceneFunction("SetString", Scene_SetStr);
		scLoader.registerSceneFunction("SetNumber", Scene_SetNum);
		scLoader.registerSceneFunction("BeginKeyFrameAnimation", Scene_BeginKeyFrameAnimation);
		scLoader.registerSceneFunction("AddKeyFrame", Scene_AddKeyFrame);
		scLoader.registerSceneFunction("EndKeyFrameAnimation", Scene_EndKeyFrameAnimation);
		scLoader.registerSceneFunction("SetBgm", Scene_SetBgm);
		scLoader.registerSceneFunction("SetVoice", Scene_SetVoice);
	}

	inline void ApplyVisualNovel(core::World2D& world, VisualNovelConfig& cfg, ScriptLoader& scLoader)
	{
		ui::ApplyAnimation(world);
		ui::ApplyButtonEx(world);
		ui::ApplyImageBoxEx(world);
		ui::ApplyKeyFramesAnimation(world);
		ui::ApplyTextBoxEx(world);
		ApplyMainTextBox(world, cfg);
		ApplyTextScene(world, scLoader);
		ApplySelectScene(world, scLoader);
		ApplyDelayScene(world, scLoader);
	}
}