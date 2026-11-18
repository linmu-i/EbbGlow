#pragma once

#include <EbbGlow/Utils/Types.h>
#include <EbbGlow/Resources/Resources.h>

#include <string>

namespace ebbglow::utils
{
	Vec2 MeasureTextSize(rsc::SharedFont, const std::string& text, float textSize, float spacing) noexcept;
	rsc::SharedFont DynamicLoadFont(const rsc::SharedFile& fontData, const std::string& text, float fontSize, rsc::FontType type = rsc::FontType::Default) noexcept;
	rsc::SharedFont DynamicLoadFont(const rsc::SharedFile& fontData, const std::vector<int>& codepoints, float fontSize, rsc::FontType type = rsc::FontType::Default) noexcept;
	std::vector<std::vector<int>> TextLineCalculateWithWordWrap(const std::string& text, float fontSize, float spacing, const rsc::SharedFont& font, float maxLength) noexcept;

	//警告，会破坏RenderTexture，调用请确保RenderTexture无其他副本
	rsc::SharedTexture2D TakeTextureFromRenderTexture(rsc::SharedRenderTexture2D&& renderTexture) noexcept;

	float GetFrameTime() noexcept;

	std::string ToUTF8Text(const std::vector<int32_t>& codepoints) noexcept;
	std::vector<int32_t> ToCodepoints(const std::string& text) noexcept;

	rsc::SharedImage LoadImageFromTexture(const rsc::SharedTexture& texture);
	rsc::SharedImage LoadImageFromTexture(const rsc::SharedRenderTexture& texture);

	Vec2 ScreenSize() noexcept;
}