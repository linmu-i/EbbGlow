#pragma once

#include <EbbGlow/UI/YUI/YUIBasic.h>
#include <EbbGlow/UI/YUI/Button/ButtonLogic.h>


namespace ebbglow::ui::yui
{
	struct FillButtonVisual
	{
		Color fillColor;
		Color borderColor;
		Color textColor;

		rsc::SharedFont font;
		float fontSize = 24.0f;
		float spacing = 1.0f;
		std::string text;

		float interpolation = 0.0f;
		float roundness = 0.2f;
	};

	class FillButtonVisualDraw : public core::DrawBase
	{
	private:
		const FillButtonVisual& visual;
		Rect rect;
		Transform transform;
		std::optional<Rect> viewPort;
	public:
		FillButtonVisualDraw(const FillButtonVisual& visual, Rect rect, Transform transform, std::optional<Rect> viewPort) :
			visual(visual), rect(rect), transform(transform), viewPort(viewPort) {
		}
		void draw() override;
	};
	class FillButtonVisualSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TransformCom>* trans;
		core::DoubleComs<ControlCom>* control;
		core::DoubleComs<ViewPortCom>* viewPort;
		core::DoubleComs<LayerCom>* layer;
		core::DoubleComs<FillButtonVisual>* visual;
		core::DoubleComs<ButtonLogic>* button;

	public:
		FillButtonVisualSystem(core::World2D& world);
		FillButtonVisualSystem(
			core::DoubleComs<TransformCom>* trans,
			core::DoubleComs<ControlCom>* control,
			core::DoubleComs<ViewPortCom>* viewPort,
			core::DoubleComs<LayerCom>* layer,
			core::DoubleComs<FillButtonVisual>* visual,
			core::DoubleComs<ButtonLogic>* button
		) : trans(trans), control(control), viewPort(viewPort), layer(layer), visual(visual), button(button) {
		}

		void update() override;
	};
}