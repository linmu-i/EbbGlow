#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/YUIBasic.h>

namespace ebbglow::ui::yui
{
	struct ImageBox
	{
		rsc::SharedTexture img;
	};

	class ImageBoxDraw : public core::DrawBase
	{
	private:
		const ImageBox& imgBox;
		std::optional<Rect> scissorRect;
		Transform transform;
	public:
		ImageBoxDraw(const ImageBox& imgBox, std::optional<Rect> scissorRect, Transform transform) :
			imgBox(imgBox), scissorRect(std::move(scissorRect)), transform(transform) {
		}
		void draw() override;
	};

	class ImageBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TransformCom>* trans;
		core::DoubleComs<ControlCom>* ctrl;
		core::DoubleComs<ViewPortCom>* viewPort;
		core::DoubleComs<LayerCom>* layer;
		core::DoubleComs<ImageBox>* imgBox;

	public:
		ImageBoxSystem(core::World2D& world);
		ImageBoxSystem(
			core::DoubleComs<TransformCom>* trans,
			core::DoubleComs<ControlCom>* ctrl,
			core::DoubleComs<ViewPortCom>* viewPort,
			core::DoubleComs<LayerCom>* layer,
			core::DoubleComs<ImageBox>* imgBox) : trans(trans), ctrl(ctrl), viewPort(viewPort), layer(layer), imgBox(imgBox) {}
		void update() override;
	};

	void ApplyImageBox(core::World2D& world);
}