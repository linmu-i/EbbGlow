#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>
#include <EbbGlow/Graphics/Graphics.h>

namespace ebbglow::visualnovel
{
	struct MainTextBoxCom
	{
		bool drawing;

		rsc::SharedFont font;

		float timeCount;

		std::string textStr;

		std::vector<rsc::SharedTexture2D> textL0;
		std::vector<rsc::SharedTexture2D> textL1;

		float textSize;
		float spacing;
		float textHeight;
		float lineSpacing;

		float totalHeightL0;
		float totalHeightL1;

		int totalPixel;
		int totalPixelL0;
		int activePixel;

		float speed;

		Vec2 pos;

		MainTextBoxCom(const std::string& textL0, const std::string& textL1, float textSize, const rsc::SharedFile& fontData, float speed, Vec2 pos, float width, Color textColor);
	};

	class MainTextBoxDraw : public core::DrawBase
	{
	private:
		rsc::SharedRenderTexture2D texture;
		Vec2 pos;

	public:
		MainTextBoxDraw(Vec2 position, rsc::SharedRenderTexture2D texture) : pos(position), texture(texture) {}
		void draw() override
		{
			gfx::DrawTextureRegion(texture, { 0, 0, float(texture.width()), -float(texture.height()) }, pos, colors::White);
		}
	};

	class MainTextBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<MainTextBoxCom>* textBoxs;
		core::Layers* layers;
		int layerDepth;
		rsc::SharedRenderTexture2D textureTmp;
		const VisualNovelConfig& cfg;

	public:
		MainTextBoxSystem(core::DoubleComs<MainTextBoxCom>* textBoxs, core::Layers* layers, int layerDepth, const VisualNovelConfig& cfg) : textBoxs(textBoxs), layers(layers), layerDepth(layerDepth), textureTmp(rsc::SharedRenderTexture(cfg.ScreenWidth, cfg.ScreenHeight)), cfg(cfg) {}

		void update() override;

		void skip(core::entity id)
		{
			textBoxs->inactive()->get(id)->drawing = false;
		}
	};

	inline void ApplyMainTextBox(core::World2D& world, const VisualNovelConfig& cfg)
	{
		world.addPool<MainTextBoxCom>();
		world.addSystem(MainTextBoxSystem(world.getDoubleBuffer<MainTextBoxCom>(), world.getUiLayer(), 10, cfg));
	}
}