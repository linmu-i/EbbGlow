#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
    void Scene_TextScene(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_Chr(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_Button(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_ImageBox(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_SetStr(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_SetNum(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_BeginKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_AddKeyFrame(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_EndKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_SetBgm(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
    void Scene_SetVoice(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
}