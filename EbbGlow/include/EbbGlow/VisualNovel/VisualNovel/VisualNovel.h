#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <EbbGlow/Utils/Resource.h>

namespace ebbglow::visualnovel
{
	struct VisualNovelConfig
	{
		uint8_t mainLanguage = 0;
		uint8_t secondaryLanguage = 1;
		uint8_t uiLanguage = 0;

		float textSpeed = 1.0f;
		float textSize = 20;

		rsc::SharedFile fontData;

		rsc::SharedTexture2D textBoxBackGround;

		rsc::SharedTexture2D chrNameBackGround;
		float chrNameOffsetX = 0.0f;

		int ScreenWidth = 0;
		int ScreenHeight = 0;

		static struct LayerDef
		{
			constexpr static uint8_t backGroundLayer = 0;
			constexpr static uint8_t illustrationLayer = 2;
			constexpr static uint8_t textBoxBackGroundLayer = 3;
			constexpr static uint8_t textBoxLayer = 4;
			constexpr static uint8_t ButtonLayer = 6;
		} LayerDefine;

		std::vector<float> volumes;

		bool showReadText = false;
		Color readTextColor = colors::White;
		std::unordered_set<std::string> readTextSet;

		float drawRatio = 0.0f;
		Vec2 drawOffset = { 0,0 };

		bool secondLanguageShow = true;

	};
}

namespace ebbglow
{
	namespace vn = visualnovel;
}