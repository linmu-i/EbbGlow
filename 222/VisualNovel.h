#pragma once

#include <unordered_set>

#include "UI.h"
#include "World.h"
#include "raylibRAII.h"

namespace visualnovel
{
	struct VisualNovelConfig
	{
		uint8_t mainLanguage;
		uint8_t secondaryLanguage;
		uint8_t uiLanguage;

		float textSpeed;
		int textSize;

		rlRAII::FileRAII fontData;
		//uint16_t fontSize;

		rlRAII::Texture2DRAII textBoxBackGround;

		rlRAII::Texture2DRAII chrNameBackGround;
		float chrNameOffsetX;

		int ScreenWidth;
		int ScreenHeight;

		constexpr static int backGroundLayer = 0;
		constexpr static int illustrationLayer = 2;
		constexpr static int textBoxLayer = 4;
		constexpr static int textBoxBackGroundLayer = 3;
		constexpr static int ButtonLayer = 6;

		bool showReadText;
		Color readTextColor;
		std::unordered_set<std::string> readTextSet;

		

	};

	const Vector2 StandardTextBoxDefaultPosition = { GetScreenWidth() / 6, GetScreenHeight() / 3 * 2};

	struct StandardTextBox
	{
		bool drawing;

		rlRAII::FontRAII font;

		float timeCount;

		std::vector<rlRAII::Texture2DRAII> textL0;
		std::vector<rlRAII::Texture2DRAII> textL1;

		float textSize;
		float spacing;
		float textHeight;
		float lineSpacing;

		float totalHeightL0;
		float totalHeightL1;

		int totalPixel;
		int activePixel = 0;

		float speed;
		
		Vector2 pos;

		
		
		StandardTextBox(const std::string& textL0, const std::string& textL1, float textSize, const rlRAII::FileRAII& fontData, float speed, Vector2 pos, float width, Color textColor) :
			textSize(textSize), spacing(textSize * 0.1f), lineSpacing(textSize * 0.3f), speed(speed), timeCount(0.0f), drawing(true),
			font(DynamicLoadFontFromMemory((textL0 + textL1).c_str(),fontData.fileName(), fontData.get(), fontData.size(), textSize * 2)),
			textHeight(MeasureTextEx(font.get(), textL0.c_str(), textSize, spacing).y), pos(pos)
		{
			std::vector<std::vector<int>> t0 = TextLineCaculateWithWordWrap(textL0.c_str(), textSize, spacing, font.get(), width);//主文本占2/3屏，1280px
			
			totalPixel = 0;



			for (auto& s : t0)
			{
				auto text = LoadUTF8(s.data(), s.size());
				Vector2 textureSize = MeasureTextEx(font.get(), text, textSize, spacing);
				RenderTexture tmpTexture = LoadRenderTexture(textureSize.x, textureSize.y);
				BeginTextureMode(tmpTexture);
				ClearBackground(BLANK);
				DrawTextCodepoints(font.get(), s.data(), s.size(), { 0,0 }, textSize, spacing, textColor);
				EndTextureMode();
				this->textL0.push_back(rlRAII::Texture2DRAII(tmpTexture.texture));
				tmpTexture.texture = {};
				UnloadRenderTexture(tmpTexture);
				UnloadUTF8(text);
			}

			std::vector<std::vector<int>> t1 = TextLineCaculateWithWordWrap(textL1.c_str(), textSize, spacing, font.get(), width);

			for (auto& s : t1)
			{
				auto text = LoadUTF8(s.data(), s.size());
				Vector2 textureSize = MeasureTextEx(font.get(), text, textSize, spacing);
				RenderTexture tmpTexture = LoadRenderTexture(textureSize.x, textureSize.y);
				BeginTextureMode(tmpTexture);
				ClearBackground(BLANK);
				DrawTextCodepoints(font.get(), s.data(), s.size(), { 0,0 }, textSize, spacing, textColor);
				EndTextureMode();
				this->textL1.push_back(rlRAII::Texture2DRAII(tmpTexture.texture));
				tmpTexture.texture = {};
				UnloadRenderTexture(tmpTexture);
				UnloadUTF8(text);
			}

			totalHeightL0 = textHeight * t0.size() + lineSpacing * t0.size();
			totalHeightL1 = textHeight * t1.size() + lineSpacing * t1.size();
			
			
			for (auto& t : this->textL0)
			{
				totalPixel += t.get().width;
			}
			for (auto& t : this->textL1)
			{
				totalPixel += t.get().width;
			}
		}
	};

	class StdTextBoxEnd : public ecs::MessageBase {};

	class StandardTextBoxDraw : public ecs::DrawBase
	{
	private:
		rlRAII::RenderTexture2DRAII texture;
		Vector2 pos;

	public:
		StandardTextBoxDraw(Vector2 position, rlRAII::RenderTexture2DRAII texture) : pos(position), texture(texture) {}
		void draw() override
		{
			DrawTextureRec(texture.get().texture, { 0, 0, float(texture.get().texture.width), -float(texture.get().texture.height) }, pos, WHITE);
		}
	};

	class StandardTextBoxSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<StandardTextBox>* textBoxs;
		ecs::Layers* layers;
		int layerDepth;
		rlRAII::RenderTexture2DRAII textureTmp;

	public:
		StandardTextBoxSystem(ecs::DoubleComs<StandardTextBox>* textBoxs, ecs::Layers* layers, int layerDepth, const VisualNovelConfig& cfg) : textBoxs(textBoxs), layers(layers), layerDepth(layerDepth), textureTmp(LoadRenderTexture(cfg.ScreenWidth, cfg.ScreenHeight)) {}

		void update() override
		{
			textBoxs->active()->forEach
			(
				[this](ecs::entity id, StandardTextBox& comActive) 
				{
					auto& comInactive = *(textBoxs->inactive()->get(id));

					comInactive = comActive;

					
					int activePixelTmp = comActive.timeCount * comActive.speed * 2000;
					
					if (!comActive.drawing)
					{
						comInactive.activePixel = comActive.totalPixel;
						activePixelTmp = comActive.totalPixel + 11;
					}
					if (comActive.activePixel < comActive.totalPixel)
					{
						comInactive.timeCount += GetFrameTime();
					}
					
					if (activePixelTmp - comActive.activePixel > 10 && comActive.activePixel < comActive.totalPixel)
					{
						comInactive.activePixel = activePixelTmp;
						int activePixel0 = activePixelTmp;
						int activePixel1 = activePixelTmp;
						
						int spacing = comActive.textHeight + comActive.lineSpacing;
						BeginTextureMode(textureTmp.get());
						ClearBackground(BLANK);
						for (int activeLine = 0; activeLine < comActive.textL0.size() && activePixel0; ++activeLine)
						{
							if (activePixel0 >= comActive.textL0[activeLine].get().width)
							{
								DrawTextureRec(comActive.textL0[activeLine].get(), { 0, 0, float(comActive.textL0[activeLine].get().width), -float(comActive.textL0[activeLine].get().height) }, { 0, float(spacing * activeLine) }, WHITE);
								activePixel0 -= comActive.textL0[activeLine].get().width;
							}
							else
							{
								DrawTextureRec(comActive.textL0[activeLine].get(), { 0, 0, float(activePixel0), -float(comActive.textL0[activeLine].get().height) }, { 0, float(spacing * activeLine) }, WHITE);
								activePixel0 = 0;
							}
						}
						for (int activeLine = 0; activeLine < comActive.textL1.size() && activePixel1; ++activeLine)
						{
							if (activePixel1 >= comActive.textL1[activeLine].get().width)
							{
								DrawTextureRec(comActive.textL1[activeLine].get(), { 0, 0, float(comActive.textL1[activeLine].get().width), -float(comActive.textL1[activeLine].get().height) }, { 0, float(spacing * activeLine + spacing * comActive.textL0.size()) }, WHITE);
								activePixel1 -= comActive.textL1[activeLine].get().width;
							}
							else
							{
								DrawTextureRec(comActive.textL1[activeLine].get(), { 0, 0, float(activePixel1), -float(comActive.textL1[activeLine].get().height) }, { 0, float(spacing * activeLine + spacing * comActive.textL0.size()) }, WHITE);
								activePixel1 = 0;
							}
						}
						EndTextureMode();
					}
					(*layers)[layerDepth].push_back(std::make_unique<StandardTextBoxDraw>(comActive.pos, textureTmp));
				}
			);
		}

		void skip(ecs::entity id)
		{
			textBoxs->inactive()->get(id)->drawing = false;
		}
	};

	void ApplyStandardTextBox(ecs::World2D& world, const VisualNovelConfig& cfg)
	{
		world.addPool<StandardTextBox>();
		world.addSystem(StandardTextBoxSystem(world.getDoubleBuffer<StandardTextBox>(), world.getUiLayer(), 10, cfg));
	}

	class SceneBase
	{
	public:
		virtual void update() = 0;
	};

	class TextScene : public SceneBase
	{
	private:
		
	};
	
}

namespace vn = visualnovel;