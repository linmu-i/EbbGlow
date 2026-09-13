#include <utility>

#include <EbbGlow/UI/YUI/Button/YUIImageButtonVisual.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::ui::yui
{
	ImageButtonVisualDraw::ImageButtonVisualDraw(Rect rect, Transform transform, std::optional<Rect> viewPort,
		const rsc::SharedTexture2D& icon, ImageButtonFitMode fitMode, Color tint) :
		rect(rect), transform(transform), viewPort(std::move(viewPort)), icon(icon), fitMode(fitMode), tint(tint)
	{
	}

	void ImageButtonVisualDraw::draw()
	{
		// 图片无效时直接跳过，避免绘制空纹理导致异常
		if (!icon.valid()) return;

		// 先应用平移，再按 pivot 应用缩放，得到按钮在屏幕空间中的目标矩形
		Rect target = rect.offsetOf(transform.position);
		if (transform.scale != 1.0f)
		{
			target = target.scaleAround(transform.pivot + transform.position, transform.scale);
		}

		float iconW = static_cast<float>(icon.width());
		float iconH = static_cast<float>(icon.height());

		// 防御：图片尺寸为 0 时无法计算缩放比例
		if (iconW <= 0.0f || iconH <= 0.0f) return;

		float destW = iconW;
		float destH = iconH;

		if (fitMode == ImageButtonFitMode::Cover)
		{
			// 完全覆盖：取较大缩放比，使图片长边先贴合容器，超出部分交由裁剪处理
			float s = std::max(target.width / iconW, target.height / iconH);
			destW = iconW * s;
			destH = iconH * s;
		}
		else if (fitMode == ImageButtonFitMode::Contain)
		{
			// 完整放入：取较小缩放比，使图片短边贴合容器，容器可能留白
			float s = std::min(target.width / iconW, target.height / iconH);
			destW = iconW * s;
			destH = iconH * s;
		}
		// Default：保持原始尺寸，不做任何缩放

		// 统一以目标矩形的中心居中放置
		Vec2 center = target.center();
		Rect dest{ center.x - destW * 0.5f, center.y - destH * 0.5f, destW, destH };

		// 裁剪区域以按钮区域为界（Cover 用于裁掉溢出部分），并与 viewPort 求交
		std::optional<Rect> scissor = target;
		if (viewPort.has_value())
		{
			scissor = scissor->intersection(*viewPort);
		}
		bool useScissor = scissor.has_value() && scissor->width > 0.0f && scissor->height > 0.0f;
		if (useScissor) BeginScissorMode(*scissor);

		// 源矩形为整张图片，旋转直接使用弧度值（Graphics 内部统一转换为角度制）
		Rect src{ 0.0f, 0.0f, iconW, iconH };
		gfx::DrawTextureRegionToRegion(icon, src, dest, Vec2{ 0.0f, 0.0f }, transform.rotation, tint);

		if (useScissor) EndScissorMode();
	}

	ImageButtonVisualSystem::ImageButtonVisualSystem(core::World2D& world)
	{
		trans = world.getDoubleBuffer<TransformCom>();
		control = world.getDoubleBuffer<ControlCom>();
		viewPort = world.getDoubleBuffer<ViewPortCom>();
		layer = world.getDoubleBuffer<LayerCom>();
		visual = world.getDoubleBuffer<ImageButtonVisual>();
		button = world.getDoubleBuffer<ButtonLogic>();
	}

	ImageButtonVisualSystem::ImageButtonVisualSystem(core::DoubleComs<TransformCom>* trans,
		core::DoubleComs<ControlCom>* control,
		core::DoubleComs<ViewPortCom>* viewPort,
		core::DoubleComs<LayerCom>* layer,
		core::DoubleComs<ImageButtonVisual>* visual,
		core::DoubleComs<ButtonLogic>* button) :
		trans(trans), control(control), viewPort(viewPort), layer(layer), visual(visual), button(button)
	{
	}

	void ImageButtonVisualSystem::update()
	{
		visual->active()->forEach([this](core::entity id, ImageButtonVisual& act)
			{
				// 遵循双缓冲约定：读取活动池，同步写入非活动池
				auto& ina = *visual->inactive()->get(id);
				ina = act;

				if (!IsVisible(control->active(), id)) return;

				auto logicPtr = button->active()->get(id);
				if (!logicPtr) return;
				auto& logic = *logicPtr;

				// 状态→图片直切映射（按下 > 悬浮 > 常态），不做任何渐变
				const rsc::SharedTexture2D* icon = &act.normalIcon;
				if (logic.isDown)
				{
					icon = &act.pressedIcon;
				}
				else if (logic.isHovered)
				{
					icon = &act.hoverIcon;
				}

				auto transform = GetFinalTransform(GetTransforms(trans->active(), id));
				auto viewPortOpt = GetFinalViewPort(viewPort->active(), trans->active(), id);

				auto* ctrl = control->active()->get(id);
				if (!ctrl || !ctrl->interactiveArea.has_value()) return;
				Rect rect = *ctrl->interactiveArea;

				auto* layerCom = layer->active()->get(id);
				if (!layerCom || !layerCom->layer) return;
				layerCom->layer->push_back(std::make_unique<ImageButtonVisualDraw>(
					rect, transform, viewPortOpt, *icon, act.fitMode, act.tint));
			});
	}
}
