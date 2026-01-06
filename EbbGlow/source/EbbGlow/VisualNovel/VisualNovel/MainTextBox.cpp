#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Functions.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::visualnovel
{
	MainTextBoxCom::MainTextBoxCom(const std::string& textL0, const std::string& textL1, float textSize, const rsc::SharedFile& fontData, float speed, Vec2 pos, float width, Color textColor, core::Layer* layer) :
		textSize(textSize), spacing(textSize * 0.1f), lineSpacing(textSize * 0.3f), speed(speed), timeCount(0.0f), drawing(true), layer(layer),
		font(utils::DynamicLoadFont(fontData, textL0 + textL1, textSize * 1.5f)), textStr(textL0), activePixel(0),
		textHeight(std::max(utils::MeasureTextSize(font, textL0, textSize, spacing).y, utils::MeasureTextSize(font, textL1, textSize, spacing).y)), pos(pos)
	{
		std::vector<std::vector<int>> t0 = utils::TextLineCalculateWithWordWrap(textL0.c_str(), textSize, spacing, font, width);

		totalPixel = 0;
		totalPixelL0 = 0;

		for (auto& s : t0)
		{
			auto text = utils::ToUTF8Text(s);
			Vec2 textureSize = utils::MeasureTextSize(font, text, textSize, spacing);
			rsc::SharedRenderTexture tmpTexture(static_cast<int>(textureSize.x), static_cast<int>(textureSize.y));
			BeginTextureMode(tmpTexture);
			//BeginShaderMode(gfx::GetDefaultAAShader());
			gfx::ClearBackground(colors::Blank);
			gfx::DrawTextCodepoints(font, s, { 0,0 }, textSize, spacing, textColor);
			//EndShaderMode();
			EndTextureMode();
			this->textL0.push_back(utils::TakeTextureFromRenderTexture(std::move(tmpTexture)));
		}

		std::vector<std::vector<int>> t1 = utils::TextLineCalculateWithWordWrap(textL1, textSize, spacing, font, width);

		for (auto& s : t1)
		{
			auto text = utils::ToUTF8Text(s);
			Vec2 textureSize = utils::MeasureTextSize(font, text, textSize, spacing);
			rsc::SharedRenderTexture2D tmpTexture(static_cast<int>(textureSize.x), static_cast<int>(textureSize.y));
			BeginTextureMode(tmpTexture);
			//BeginShaderMode(gfx::GetDefaultAAShader());
			gfx::ClearBackground(colors::Blank);
			gfx::DrawTextCodepoints(font, s, { 0,0 }, textSize, spacing, textColor);
			//EndShaderMode();
			EndTextureMode();
			this->textL1.push_back(utils::TakeTextureFromRenderTexture(std::move(tmpTexture)));
		}

		totalHeightL0 = textHeight * t0.size() + lineSpacing * t0.size();
		totalHeightL1 = textHeight * t1.size() + lineSpacing * t1.size();


		for (auto& t : this->textL0)
		{
			totalPixel += t.width();
			totalPixelL0 += t.width();
		}
		for (auto& t : this->textL1)
		{
			totalPixel += t.width();
		}
	}

	void MainTextBoxSystem::update()
	{
		textBoxs->active()->forEach
		(
			[this](core::entity id, MainTextBoxCom& comActive)
			{
				if (comActive.activePixel == 0)
				{
					BeginTextureMode(textureTmp);
					gfx::ClearBackground(colors::Blank);
					EndTextureMode();
				}
				auto& comInactive = *(textBoxs->inactive()->get(id));

				int activePixelTmp = static_cast<int>(comActive.timeCount * comActive.speed * 2000);

				if (!comActive.drawing)
				{
					comInactive.activePixel = comActive.totalPixel;
					activePixelTmp = comActive.totalPixel + 11;
				}
				if (comActive.activePixel < comActive.totalPixel)
				{
					comInactive.timeCount += utils::GetFrameTime();
				}

				if (activePixelTmp - comActive.activePixel > 10 && comActive.activePixel < comActive.totalPixel)
				{
					comInactive.activePixel = activePixelTmp;
					int activePixel0 = activePixelTmp;
					int activePixel1 = activePixelTmp;

					int spacing = static_cast<int>(comActive.textHeight + comActive.lineSpacing);
					BeginTextureMode(textureTmp);
					BeginShaderMode(gfx::GetDefaultAAShader());
					for (int activeLine = 0; activeLine < comActive.textL0.size() && activePixel0; ++activeLine)
					{
						if (activePixel0 >= comActive.textL0[activeLine].width())
						{
							gfx::DrawTextureRegion(comActive.textL0[activeLine], { 0, 0, float(comActive.textL0[activeLine].width()), -float(comActive.textL0[activeLine].height()) }, { 0, float(spacing * activeLine) }, colors::White);
							activePixel0 -= comActive.textL0[activeLine].width();
						}
						else
						{
							gfx::DrawTextureRegion(comActive.textL0[activeLine], { 0, 0, float(activePixel0), -float(comActive.textL0[activeLine].height()) }, { 0, float(spacing * activeLine) }, colors::White);
							activePixel0 = 0;
						}
					}
					if (cfg.secondLanguageShow)
					{
						for (int activeLine = 0; activeLine < comActive.textL1.size() && activePixel1; ++activeLine)
						{
							if (activePixel1 >= comActive.textL1[activeLine].width())
							{
								gfx::DrawTextureRegion(comActive.textL1[activeLine], {0, 0, float(comActive.textL1[activeLine].width()), -float(comActive.textL1[activeLine].height())}, {0, float(spacing * activeLine + spacing * comActive.textL0.size())}, colors::White);
								activePixel1 -= comActive.textL1[activeLine].width();
							}
							else
							{
								gfx::DrawTextureRegion(comActive.textL1[activeLine], { 0, 0, float(activePixel1), -float(comActive.textL1[activeLine].height()) }, { 0, float(spacing * activeLine + spacing * comActive.textL0.size()) }, colors::White);
								activePixel1 = 0;
							}
						}
					}
					else if (comActive.activePixel > comActive.totalPixelL0)
					{
						comInactive.drawing = false;
						comInactive.activePixel = comActive.totalPixel + 11;
					}
					EndShaderMode();
					EndTextureMode();
				}
				(*comActive.layer).push_back(std::make_unique<MainTextBoxDraw>(comActive.pos, textureTmp));
			}
		);

	}
}