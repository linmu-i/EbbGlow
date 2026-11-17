#pragma once

#include <optional>

#include <EbbGlow/UI/YUI/YUIBasic.h>
#include <EbbGlow/UI/YUI/Button/ButtonLogic.h>

namespace ebbglow::ui::yui
{
	/// 图片按钮的图片适配模式：决定图片如何缩放到按钮区域。
	enum class ImageButtonFitMode : uint8_t
	{
		Contain,  ///< 保持长宽比缩放并居中，完整放入容器（容器可能出现留白）
		Cover,    ///< 保持长宽比缩放并居中，完全覆盖容器（超出部分被裁剪）
		Default   ///< 不缩放，按图片原始尺寸居中绘制
	};

	/// 图片按钮视觉组件：持有常态/悬浮/按下三张图片，按交互状态直接切换（无渐变）。
	/// 状态优先级为「按下 > 悬浮 > 常态」。
	struct ImageButtonVisual
	{
		rsc::SharedTexture2D normalIcon;   ///< 常态图片
		rsc::SharedTexture2D hoverIcon;    ///< 悬浮图片
		rsc::SharedTexture2D pressedIcon;  ///< 按下图片
		ImageButtonFitMode fitMode = ImageButtonFitMode::Cover; ///< 图片适配模式
		Color tint = colors::White;        ///< 叠加色调（默认白色即不改变原色）
	};

	/// 图片按钮绘制对象：在绘制阶段按 fitMode 将当前状态图片适配到按钮区域。
	/// 持有值语义的已选图片与变换，避免绘制阶段引用活动池组件的生命周期问题。
	class ImageButtonVisualDraw : public core::DrawBase
	{
	private:
		Rect rect;                          ///< 交互区域（等同 ControlCom.interactiveArea）
		Transform transform;                ///< 最终变换（position/pivot/rotation/scale）
		std::optional<Rect> viewPort;       ///< 最终裁剪矩形（可空）
		rsc::SharedTexture2D icon;          ///< 已由 System 选定的当前状态图片
		ImageButtonFitMode fitMode;         ///< 图片适配模式
		Color tint;                         ///< 叠加色调

	public:
		/// 构造图片按钮绘制对象。
		/// @param rect 交互区域矩形。
		/// @param transform 该按钮的最终变换。
		/// @param viewPort 最终裁剪矩形（可能为空）。
		/// @param icon 当前交互状态对应的图片。
		/// @param fitMode 图片适配模式。
		/// @param tint 叠加色调。
		ImageButtonVisualDraw(Rect rect, Transform transform, std::optional<Rect> viewPort,
			const rsc::SharedTexture2D& icon, ImageButtonFitMode fitMode, Color tint);

		void draw() override;
	};

	/// 图片按钮视觉系统：每帧读取 ButtonLogic 状态，选定图片并提交绘制。
	class ImageButtonVisualSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TransformCom>* trans;        ///< 变换组件双缓冲
		core::DoubleComs<ControlCom>* control;        ///< 控制组件双缓冲
		core::DoubleComs<ViewPortCom>* viewPort;      ///< 视口组件双缓冲
		core::DoubleComs<LayerCom>* layer;            ///< 图层组件双缓冲
		core::DoubleComs<ImageButtonVisual>* visual;  ///< 图片按钮视觉组件双缓冲
		core::DoubleComs<ButtonLogic>* button;        ///< 按钮交互状态组件双缓冲

	public:
		/// 从世界对象中获取所需全部组件池。
		/// @param world 当前世界对象。
		explicit ImageButtonVisualSystem(core::World2D& world);

		/// 使用显式传入的组件池构造（便于子世界或测试场景复用）。
		/// @param trans 变换组件双缓冲。
		/// @param control 控制组件双缓冲。
		/// @param viewPort 视口组件双缓冲。
		/// @param layer 图层组件双缓冲。
		/// @param visual 图片按钮视觉组件双缓冲。
		/// @param button 按钮交互状态组件双缓冲。
		ImageButtonVisualSystem(core::DoubleComs<TransformCom>* trans,
			core::DoubleComs<ControlCom>* control,
			core::DoubleComs<ViewPortCom>* viewPort,
			core::DoubleComs<LayerCom>* layer,
			core::DoubleComs<ImageButtonVisual>* visual,
			core::DoubleComs<ButtonLogic>* button);

		void update() override;
	};
}
