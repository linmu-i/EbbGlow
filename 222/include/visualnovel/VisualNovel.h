#pragma once

#include <unordered_set>

#include <ui/UI.h>
#include <core/World.h>
#include <utils/Resource.h>

namespace visualnovel
{
	struct VisualNovelConfig
	{
		uint8_t mainLanguage = 0;
		uint8_t secondaryLanguage = 1;
		uint8_t uiLanguage = 0;

		float textSpeed = 1.0f;
		int textSize = 20;

		rsc::SharedFile fontData;

		rsc::SharedTexture2D textBoxBackGround;

		rsc::SharedTexture2D chrNameBackGround;
		float chrNameOffsetX = 0.0f;

		int ScreenWidth = 0;
		int ScreenHeight = 0;

		static struct LayerDef
		{
			constexpr static int backGroundLayer = 0;
			constexpr static int illustrationLayer = 2;
			constexpr static int textBoxBackGroundLayer = 3;
			constexpr static int textBoxLayer = 4;
			constexpr static int ButtonLayer = 6;
		} LayerDefine;
		

		bool showReadText = false;
		Color readTextColor = WHITE;
		std::unordered_set<std::string> readTextSet;

		float drawRatio = 0.0f;
		Vector2 drawOffset = { 0,0 };

	};
	
	const Vector2 StandardTextBoxDefaultPosition = { GetScreenWidth() / 6, GetScreenHeight() / 3 * 2};

	struct StandardTextBox
	{
		bool drawing;

		rsc::SharedFont font;

		float timeCount;

		std::vector<rsc::SharedTexture2D> textL0;
		std::vector<rsc::SharedTexture2D> textL1;

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

		
		
		StandardTextBox(const std::string& textL0, const std::string& textL1, float textSize, const rsc::SharedFile& fontData, float speed, Vector2 pos, float width, Color textColor) :
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
				this->textL0.push_back(rsc::SharedTexture2D(tmpTexture.texture));
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
				this->textL1.push_back(rsc::SharedTexture2D(tmpTexture.texture));
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
		rsc::SharedRenderTexture2D texture;
		Vector2 pos;

	public:
		StandardTextBoxDraw(Vector2 position, rsc::SharedRenderTexture2D texture) : pos(position), texture(texture) {}
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
		rsc::SharedRenderTexture2D textureTmp;

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
	
}

namespace vn = visualnovel;