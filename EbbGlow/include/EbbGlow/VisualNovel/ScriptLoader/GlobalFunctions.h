#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	void Global_String(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
	void Global_Number(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
	void Global_Include(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
	void Global_BeginScene(ScriptLoader* loader, const std::vector<std::string>& args) noexcept;
}