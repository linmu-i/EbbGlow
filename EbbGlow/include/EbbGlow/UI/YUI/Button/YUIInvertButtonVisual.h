#pragma once

#include <optional>
#include <string>

#include <EbbGlow/UI/YUI/YUIBasic.h>
#include <EbbGlow/UI/YUI/Button/YUIButtonLogic.h>

namespace ebbglow::ui::yui
{
	/// 反色按钮主题：决定常态/按下态的填充与文字配色取向。
	enum class InvertButtonTheme : uint8_t
	{
		Dark,    ///< 深色主题
		Light    ///< 浅色主题
	};

	/// 反色按钮视觉组件：三色（deep D / mid M / light L）+ 主题，在「标准 / 悬浮 / 按下」三态间切换配色。
	/// 边框恒为 deep 色；状态优先级为「按下 > 悬浮 > 标准」。配色规则：
	/// - dark 主题：常态 边框D/填充D/文字L；悬浮 边框D/填充M/文字D；按下 边框D/填充L/文字D
	/// - light 主题：常态 边框D/填充L/文字D；悬浮 边框D/填充M/文字D；按下 边框D/填充D/文字L
	struct InvertButtonVisual
	{
		Color deepColor;                           ///< 深色 D：边框恒用；dark 主题常态填充、light 主题按下填充
		Color midColor;                            ///< 中间色 M：悬浮态填充
		Color lightColor;                          ///< 浅色 L：light 主题常态填充、dark 主题按下填充
		InvertButtonTheme theme = InvertButtonTheme::Dark; ///< 主题（默认深色）

		rsc::SharedFont font;                     ///< 按钮文字字体
		float fontSize = 24.0f;                   ///< 文字大小
		float spacing = 1.0f;                     ///< 文字字距
		std::string text;                         ///< 按钮文字内容
		float roundness = 0.2f;                   ///< 圆角比例（与 FillButtonVisual 一致）

		float interpolation = 0.0f;               ///< 状态过渡进度：0=标准，0.5=悬浮，1=按下
		float transitionSpeed = 15.0f;            ///< 过渡速度（每秒进度值，越大越快）
	};

	/// 将已解析好的三态配色提交到渲染层。
	/// 持有的是「值语义」的最终配色，避免绘制阶段引用活动池组件导致的生命周期问题。
	class InvertButtonVisualDraw : public core::DrawBase
	{
	private:
		Rect rect;                                ///< 交互区域（等同 ControlCom.interactiveArea）
		Transform transform;                      ///< 最终变换（position/pivot/rotation/scale）
		std::optional<Rect> viewPort;             ///< 最终裁剪矩形（可空）
		Color fill;                               ///< 已解析的填充色
		Color border;                             ///< 已解析的边框色
		Color textColor;                          ///< 已解析的文字色
		rsc::SharedFont font;                     ///< 按钮文字字体
		float fontSize;                           ///< 文字大小
		float spacing;                            ///< 文字字距
		std::string text;                         ///< 按钮文字内容
		float roundness;                          ///< 圆角比例

	public:
		/// 构造反色按钮绘制对象。
		/// @param rect 交互区域矩形。
		/// @param transform 该按钮的最终变换。
		/// @param viewPort 最终裁剪矩形（可能为空）。
		/// @param fill 已解析的填充色。
		/// @param border 已解析的边框色。
		/// @param textColor 已解析的文字色。
		/// @param font 按钮文字字体。
		/// @param fontSize 文字大小。
		/// @param spacing 文字字距。
		/// @param text 按钮文字内容。
		/// @param roundness 圆角比例。
		InvertButtonVisualDraw(Rect rect, Transform transform, std::optional<Rect> viewPort,
			Color fill, Color border, Color textColor,
			const rsc::SharedFont& font, float fontSize, float spacing,
			const std::string& text, float roundness);
		void draw() override;
	};

	/// 反色按钮视觉系统：每帧读取 ButtonLogic 的交互状态，解析三态配色并提交绘制。
	class InvertButtonVisualSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TransformCom>* trans;        ///< 变换组件双缓冲
		core::DoubleComs<ControlCom>* control;        ///< 控制组件双缓冲
		core::DoubleComs<ViewPortCom>* viewPort;      ///< 视口组件双缓冲
		core::DoubleComs<LayerCom>* layer;            ///< 图层组件双缓冲
		core::DoubleComs<InvertButtonVisual>* visual; ///< 反色按钮视觉组件双缓冲
		core::DoubleComs<ButtonLogic>* button;        ///< 按钮交互状态组件双缓冲

	public:
		/// 从世界对象中获取所需全部组件池。
		/// @param world 当前世界对象。
		explicit InvertButtonVisualSystem(core::World2D& world);

		/// 使用显式传入的组件池构造（便于子世界或测试场景复用）。
		/// @param trans 变换组件双缓冲。
		/// @param control 控制组件双缓冲。
		/// @param viewPort 视口组件双缓冲。
		/// @param layer 图层组件双缓冲。
		/// @param visual 反色按钮视觉组件双缓冲。
		/// @param button 按钮交互状态组件双缓冲。
		InvertButtonVisualSystem(core::DoubleComs<TransformCom>* trans,
			core::DoubleComs<ControlCom>* control,
			core::DoubleComs<ViewPortCom>* viewPort,
			core::DoubleComs<LayerCom>* layer,
			core::DoubleComs<InvertButtonVisual>* visual,
			core::DoubleComs<ButtonLogic>* button);

		void update() override;
	};
}
