#pragma once

#include <functional>

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/UILayout/Layout.h>
#include <EbbGlow/UI/YUI/Button/YUIInvertButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/YUIImageButtonVisual.h>

namespace ebbglow::ui::yui::layout
{
	struct [[nodiscard]] UICreator
	{
		core::World2D* world = nullptr;
		std::vector<core::entity> idList = {};

		std::vector<core::entity> operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Column
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Row
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Spacer
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()();
	};

	struct [[nodiscard]] Box
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Button
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		Color color = 0x2563ebff;
		Color borderColor = 0x00000000;
		Color textColor = 0xffffffff;

		uint8_t layerDepth = 8;

		rsc::SharedFont font;
		float fontSize = 24.0f;
		float spacing = 1.0f;

		std::string text;

		float roundness = 0.2f;

		std::function<void(core::World2D&, core::entity)> onClick;

		void operator()();
	};

	/// 反色按钮 DSL：提供深色主题色 / 浅色主题色 / 填充色三种颜色，按交互状态自动切换配色。
	/// 用法与 Button 完全一致，可置于 UICreator / Column / Row / Box 的布局回调中。
	struct [[nodiscard]] InvertButton
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		Color deepColor = 0x1f2937ff;		///< 深色 D：边框恒用；dark 主题常态填充、light 主题按下填充
		Color midColor = 0x9ca3afff;		///< 中间色 M：悬浮态填充
		Color lightColor = 0xf9fafbff;		///< 浅色 L：light 主题常态填充、dark 主题按下填充
		InvertButtonTheme theme = InvertButtonTheme::Dark; ///< 主题（默认深色）

		uint8_t layerDepth = 8;

		rsc::SharedFont font;
		float fontSize = 24.0f;
		float spacing = 1.0f;

		std::string text;

		float roundness = 0.2f;

		std::function<void(core::World2D&, core::entity)> onClick;

		void operator()();
	};

	/// 图片按钮 DSL：提供常态/悬浮/按下三张图片，按交互状态直接切换（无渐变）。
	/// 用法与 Button / InvertButton 一致，可置于 UICreator / Column / Row / Box 的布局回调中。
	struct [[nodiscard]] ImageButton
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		rsc::SharedTexture2D normalIcon;   ///< 常态图片
		rsc::SharedTexture2D hoverIcon;    ///< 悬浮图片
		rsc::SharedTexture2D pressedIcon;  ///< 按下图片
		ImageButtonFitMode fitMode = ImageButtonFitMode::Cover; ///< 图片适配模式
		Color tint = colors::White;        ///< 叠加色调

		uint8_t layerDepth = 8;

		std::function<void(core::World2D&, core::entity)> onClick;

		void operator()();
	};

	struct [[nodiscard]] Text
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;
		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;
		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		Color textColor = 0x2563ebff;
		uint8_t layerDepth = 8;
		rsc::SharedFont font;
		float textSize = 24.0f;
		float spacing = 1.0f;
		std::string text;

		void operator()();
	};
}