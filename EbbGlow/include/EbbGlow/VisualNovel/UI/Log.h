#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	struct ItemCom
	{
		std::string text;
		std::string exText;
		std::string voicePath;
		std::string sceneName;

		Vec2 position;
		float scale;
		const VisualNovelConfig& cfg;
		rsc::SharedFont font;

		core::entity jumpButtonId;
		core::entity voiceButtonId;

		ItemCom(const std::string& text, const std::string& exText, const std::string& voicePath, const std::string& sceneName, Vec2 position, float scale, const VisualNovelConfig& cfg)
			: text(text), exText(exText), voicePath(voicePath), sceneName(sceneName), position(position), scale(scale), cfg(cfg) {
		}
	};

	class ItemDraw : public core::DrawBase
	{
	private:
		const ItemCom& com;

	public:
		ItemDraw(const ItemCom& com) : com(com) {}
		void draw() override;
	};

	struct LogCom {};

	class LogSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<LogCom>* coms;
		ScriptLoader* scLoader;

		core::Layers layersBuf;
		rsc::SharedRenderTexture2D textureBuf;

	public:
		LogSystem(ScriptLoader* scLoader) : scLoader(scLoader), world(&scLoader->world) {}

		void update() override;
	};
}