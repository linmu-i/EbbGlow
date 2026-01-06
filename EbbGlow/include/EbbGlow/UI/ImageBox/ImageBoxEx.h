#pragma once

#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Core/World.h>

namespace ebbglow::ui
{
	struct ImageBoxExCom
	{
		rsc::SharedTexture2D img;
		Vec2 position;
		Vec2 origin;
		float scale;
		float rotation;
		Color tint;
		core::Layer* layer;

		ImageBoxExCom(const rsc::SharedTexture2D& img, Vec2 position, uint8_t layerDepth, core::Layers* layer, float scale = 1.0f, float rotation = 0.0f, Vec2 origin = { 0.0f, 0.0f }, Color tint = colors::White) :
			img(img), position(position), layer(&(*layer)[layerDepth]), scale(scale), rotation(rotation), origin(origin), tint(tint) {
		}

		ImageBoxExCom(const rsc::SharedTexture2D& img, Vec2 position, core::Layer* layer, float scale = 1.0f, float rotation = 0.0f, Vec2 origin = { 0.0f, 0.0f }, Color tint = colors::White) :
			img(img), position(position), layer(layer), scale(scale), rotation(rotation), origin(origin), tint(tint) {
		}
	};

	class ImageBoxExDraw : public core::DrawBase
	{
	private:
		const ImageBoxExCom& imgBox;

	public:
		ImageBoxExDraw(const ImageBoxExCom& imgBox) : imgBox(imgBox) {}
		void draw() override;
	};

	class ImageBoxExSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ImageBoxExCom>* imgBoxes;

	public:
		ImageBoxExSystem(core::World2D& world) : imgBoxes(world.getDoubleBuffer<ImageBoxExCom>()) {}
		ImageBoxExSystem(core::DoubleComs<ImageBoxExCom>* imgBoxes) : imgBoxes(imgBoxes) {}
		void update() override;
	};

	inline void ApplyImageBoxEx(core::World2D& world)
	{
		world.addPool<ImageBoxExCom>();
		world.addSystem(ImageBoxExSystem(world));
	}
}