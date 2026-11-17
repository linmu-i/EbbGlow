#include <EbbGlow/UI/YUI/Button/FillButtonVisual.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Functions.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::ui::yui
{
	void FillButtonVisualDraw::draw()
	{
		if (viewPort.has_value())
		{
			BeginScissorMode(*viewPort);
		}
		gfx::DrawRectRounded(rect.offsetOf(transform.position), visual.roundness, visual.fillColor, transform.scale, transform.rotation, transform.pivot);
		gfx::DrawRectRoundedLines(rect.offsetOf(transform.position), visual.roundness, visual.borderColor, 2.0f, transform.scale, transform.rotation, transform.pivot);
		Vec2 textSize = utils::MeasureTextSize(visual.font, visual.text, visual.fontSize * transform.scale, visual.spacing * transform.scale);

		Rect rectScaled = rect.scaleAround(transform.pivot, transform.scale);
		Vec2 textPos = rectScaled.center() - (textSize * 0.5f);
		Vec2 textPivot = transform.pivot - textPos;
		textPos += transform.position;

		gfx::DrawText(visual.font, visual.text, textPos, visual.fontSize * transform.scale, visual.spacing * transform.scale, visual.textColor, textPivot, transform.rotation);
		gfx::DrawRectRounded(rect.offsetOf(transform.position).extendOf({ 2.0f, 2.0f }).offsetOf({-1.0f, -1.0f}), visual.roundness, Color{ 255, 255, 255, static_cast<uint8_t>(255 * visual.interpolation) }, transform.scale, transform.rotation, transform.pivot);
		if (viewPort.has_value())
		{
			EndScissorMode();
		}
	}

	FillButtonVisualSystem::FillButtonVisualSystem(core::World2D& world)
	{
		trans = world.getDoubleBuffer<TransformCom>();
		control = world.getDoubleBuffer<ControlCom>();
		viewPort = world.getDoubleBuffer<ViewPortCom>();
		layer = world.getDoubleBuffer<LayerCom>();
		visual = world.getDoubleBuffer<FillButtonVisual>();
		button = world.getDoubleBuffer<ButtonLogic>();
	}

	void FillButtonVisualSystem::update()
	{
		visual->active()->forEach([this](core::entity id, FillButtonVisual& act)
			{
				auto& ina = *visual->inactive()->get(id);
				ina = act;
				if (!IsVisible(control->active(), id)) return;

				

				auto logicPtr = button->active()->get(id);
				if (!logicPtr) return;
				auto& logic = *logicPtr;

				if (logic.isDown || logic.isHovered)
				{
					ina.interpolation += utils::GetFrameTime() * 10.0f;
				}
				else
				{
					ina.interpolation -= utils::GetFrameTime() * 10.0f;
				}
				if (logic.isDown)
				{
					ina.interpolation = std::clamp(ina.interpolation, 0.0f, 0.8f);
				}
				else
				{
					ina.interpolation = std::clamp(ina.interpolation, 0.0f, 0.5f);
				}

				auto transform = GetFinalTransform(GetTransforms(trans->active(), id));
				auto viewPortOpt = GetFinalViewPort(viewPort->active(), trans->active(), id);
				auto rect = control->active()->get(id)->interactiveArea;

				auto* layerCom = layer->active()->get(id);
				layerCom->layer->add(std::make_unique<FillButtonVisualDraw>(act, *rect, transform, viewPortOpt), layerCom->priority);
			});
	}
}