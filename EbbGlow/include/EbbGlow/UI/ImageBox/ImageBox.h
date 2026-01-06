#pragma once

#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Core/World.h>

namespace ebbglow::ui
{
	struct ImageBoxCom
	{
		rsc::SharedTexture2D img;
		Vec2 position;
		float scale;
		uint8_t layerDepth;
	};

	class ImageBoxDraw : public core::DrawBase
	{
	private:
		const ImageBoxCom& imgBox;

	public:
		ImageBoxDraw(const ImageBoxCom& imgBox) : imgBox(imgBox){}
		void draw() override;
	};

	class ImageBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ImageBoxCom>* imgBoxes;
		core::Layers* layer;

	public:
		ImageBoxSystem(core::World2D& world) : imgBoxes(world.getDoubleBuffer<ImageBoxCom>()), layer(world.getUiLayer()) {}
		void update() override;
	};

	inline void ApplyImageBox(core::World2D& world)
	{
		world.addPool<ImageBoxCom>();
		world.addSystem(ImageBoxSystem(world));
	}
}