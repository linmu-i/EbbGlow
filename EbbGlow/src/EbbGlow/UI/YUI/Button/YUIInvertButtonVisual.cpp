#include <utility>

#include <EbbGlow/UI/YUI/Button/YUIInvertButtonVisual.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Functions.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::ui::yui
{
	//颜色逐通道线性插值（t 夹取到 [0,1]），用于状态间平滑过渡
	static Color LerpColor(Color a, Color b, float t) noexcept
	{
		t = std::clamp(t, 0.0f, 1.0f);
		return Color(
			static_cast<uint8_t>(a.r + (b.r - a.r) * t),
			static_cast<uint8_t>(a.g + (b.g - a.g) * t),
			static_cast<uint8_t>(a.b + (b.b - a.b) * t),
			static_cast<uint8_t>(a.a + (b.a - a.a) * t));
	}

	InvertButtonVisualDraw::InvertButtonVisualDraw(Rect rect, Transform transform, std::optional<Rect> viewPort,
		Color fill, Color border, Color textColor,
		const rsc::SharedFont& font, float fontSize, float spacing,
		const std::string& text, float roundness) :
		rect(rect), transform(transform), viewPort(std::move(viewPort)), fill(fill), border(border), textColor(textColor),
		font(font), fontSize(fontSize), spacing(spacing), text(text), roundness(roundness)
	{
	}

	void InvertButtonVisualDraw::draw()
	{
		if (viewPort.has_value())
		{
			BeginScissorMode(*viewPort);
		}

		// 圆角矩形填充（配色已由 System 按三态解析完毕）
		gfx::DrawRectRounded(rect.offsetOf(transform.position), roundness, fill, transform.scale, transform.rotation, transform.pivot);
		// 圆角矩形边框，线宽与 FillButtonVisual 保持一致
		gfx::DrawRectRoundedLines(rect.offsetOf(transform.position), roundness, border, 2.0f, transform.scale, transform.rotation, transform.pivot);

		// 仅在字体有效时绘制文字，避免无效字体导致的崩溃
		if (font.valid())
		{
			Vec2 textSize = utils::MeasureTextSize(font, text, fontSize * transform.scale, spacing * transform.scale);

			Rect rectScaled = rect.scaleAround(transform.pivot, transform.scale);
			Vec2 textPos = rectScaled.center() - (textSize * 0.5f);
			Vec2 textPivot = transform.pivot - textPos;
			textPos += transform.position;

			gfx::DrawText(font, text, textPos, fontSize * transform.scale, spacing * transform.scale, textColor, textPivot, transform.rotation);
		}

		if (viewPort.has_value())
		{
			EndScissorMode();
		}
	}

	InvertButtonVisualSystem::InvertButtonVisualSystem(core::World2D& world)
	{
		trans = world.getDoubleBuffer<TransformCom>();
		control = world.getDoubleBuffer<ControlCom>();
		viewPort = world.getDoubleBuffer<ViewPortCom>();
		layer = world.getDoubleBuffer<LayerCom>();
		visual = world.getDoubleBuffer<InvertButtonVisual>();
		button = world.getDoubleBuffer<ButtonLogic>();
	}

	InvertButtonVisualSystem::InvertButtonVisualSystem(core::DoubleComs<TransformCom>* trans,
		core::DoubleComs<ControlCom>* control,
		core::DoubleComs<ViewPortCom>* viewPort,
		core::DoubleComs<LayerCom>* layer,
		core::DoubleComs<InvertButtonVisual>* visual,
		core::DoubleComs<ButtonLogic>* button) :
		trans(trans), control(control), viewPort(viewPort), layer(layer), visual(visual), button(button)
	{
	}

	void InvertButtonVisualSystem::update()
	{
		visual->active()->forEach([this](core::entity id, InvertButtonVisual& act)
			{
				// 遵循双缓冲约定：读取活动池，同步写入非活动池
				auto& ina = *visual->inactive()->get(id);
				ina = act;

				if (!IsVisible(control->active(), id)) return;

				auto logicPtr = button->active()->get(id);
				if (!logicPtr) return;
				auto& logic = *logicPtr;

				// 目标过渡值：按下=1.0，悬浮=0.5，标准=0.0（按下优先于悬浮）
				float target = logic.isDown ? 1.0f : (logic.isHovered ? 0.5f : 0.0f);

				// 每帧朝目标逼近形成渐变；在 inactive 缓冲累加，帧末 swap 后生效
				float step = utils::GetFrameTime() * act.transitionSpeed;
				ina.interpolation = std::clamp(
					ina.interpolation + std::clamp(target - ina.interpolation, -step, step),
					0.0f, 1.0f);

				// 三态基准配色：边框恒为 deep（故三态边框一致）；悬浮态两主题一致
				const Color normalBorder = act.deepColor;
				const Color hoverBorder = act.deepColor;
				const Color pressBorder = act.deepColor;
				const Color hoverFill = act.midColor;
				const Color hoverText = act.deepColor;

				// 常态/按下态的填充与文字随主题切换
				Color normalFill;
				Color normalText;
				Color pressFill;
				Color pressText;
				if (act.theme == InvertButtonTheme::Dark)
				{
					// 深色主题：常态 填充D/文字L；按下 填充L/文字D
					normalFill = act.deepColor;  normalText = act.lightColor;
					pressFill = act.lightColor;  pressText = act.deepColor;
				}
				else
				{
					// 浅色主题：常态 填充L/文字D；按下 填充D/文字L
					normalFill = act.lightColor; normalText = act.deepColor;
					pressFill = act.deepColor;   pressText = act.lightColor;
				}

				// 分段混合：0→0.5 为 标准→悬浮；0.5→1 为 悬浮→按下
				Color fill;
				Color border;
				Color textColor;
				float t = ina.interpolation;
				if (t <= 0.5f)
				{
					float k = t / 0.5f;
					fill = LerpColor(normalFill, hoverFill, k);
					border = LerpColor(normalBorder, hoverBorder, k);
					textColor = LerpColor(normalText, hoverText, k);
				}
				else
				{
					float k = (t - 0.5f) / 0.5f;
					fill = LerpColor(hoverFill, pressFill, k);
					border = LerpColor(hoverBorder, pressBorder, k);
					textColor = LerpColor(hoverText, pressText, k);
				}

				auto transform = GetFinalTransform(GetTransforms(trans->active(), id));
				auto viewPortOpt = GetFinalViewPort(viewPort->active(), trans->active(), id);

				auto* ctrl = control->active()->get(id);
				if (!ctrl || !ctrl->interactiveArea.has_value()) return;
				Rect rect = *ctrl->interactiveArea;

				auto* layerCom = layer->active()->get(id);
				if (!layerCom || !layerCom->layer) return;
				layerCom->layer->push_back(std::make_unique<InvertButtonVisualDraw>(
					rect, transform, viewPortOpt, fill, border, textColor,
					act.font, act.fontSize, act.spacing, act.text, act.roundness));
			});
	}
}
