//整体流程如下
// 1.外层容器通过回调，调用内层创建内层，内层传入回调							外层回调将外层压入栈顶
// 2.内层创建完毕后，外层回调获取尺寸信息，根据自己的Size填充内层的Size			栈顶为外层
// 3.外层调用内层的回调															内层回调将内层压入栈顶

#include <EbbGlow/UI/YUI/UILayout/LayoutCom.h>
#include <EbbGlow/UI/YUI/UILayout/ContextStack.h>
#include <EbbGlow/UI/YUI/YUIBasic.h>
#include <EbbGlow/UI/YUI/Button/Button.h>
#include <EbbGlow/UI/YUI/Button/ButtonCallback.h>
#include <EbbGlow/UI/YUI/Button/InvertButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/ImageButtonVisual.h>
#include <EbbGlow/UI/YUI/TextBox/TextBox.h>

namespace ebbglow::ui::yui::layout
{
	std::vector<core::entity> UICreator::operator()(std::function<void()> func)
	{
		if (!world) return {};
		InitContextStack(*world, std::move(idList), beginDrawPriority);

		Vec2 ScSize = utils::ScreenSize();

		ContextStackGuard guard{ core::entity() };
		
		Box
		{
			.widthMode = SizeMode::Pixels,
			.widthValue = ScSize.x,
			.heightMode = SizeMode::Pixels,
			.heightValue = ScSize.y
		}(func);

		auto& children = GetContextStack().getCurrentChildren();
		for (auto& child : children)
		{
			child.callback(child);
		}

		return GetContextStack().getIdList();
	}

	std::vector<core::entity> UICreator::operator>>(std::function<void()> func)
	{
		return this->operator()(func);
	}

	void Column::operator()(std::function<void()> func)
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();

		auto id = context.getNextId();
		auto parentId = context.getCurrentParent();

		TransformCom trans{};
		TransformAttachTo(trans, parentId);

		ControlCom ctrl{};
		ControlAttachTo(ctrl, parentId);

		ViewPortCom vp{};
		ViewPortAttachTo(vp, parentId);

		world.createUnit(id, trans, ctrl, vp);

		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		Vec2 size{};
		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;

		auto callback = [func](ChildData& selfData)
		{
			auto& world = GetContextStack().getWorld();

			ContextStackGuard guard(selfData.childId);

			func();

			auto& children = GetContextStack().getCurrentChildren();
			
			float totalWeightHeight = 0.0f;
			float totalPixelOrAutoHeight = 0.0f;

			for (const auto& child : children)
			{
				if (child.layout.heightMode == SizeMode::Weight)
				{
					totalWeightHeight += child.layout.heightValue;
				}
				else
				{
					totalPixelOrAutoHeight += child.size.y;
				}
			}

			float remainingHeight = selfData.size.y - totalPixelOrAutoHeight;

			float heightCount = 0.0f;

			for (auto& child : children)
			{
				if (child.layout.heightMode == SizeMode::Weight)
				{
					child.size.y = (child.layout.heightValue / totalWeightHeight) * remainingHeight;
				}
				if (child.layout.widthMode == SizeMode::Weight)
				{
					child.size.x = child.layout.widthValue * selfData.size.x;
				}

				heightCount += child.size.y;
				auto transformPtr = world.getWaitAdd<TransformCom>(child.childId);
				if (!transformPtr) continue;
				auto& transform = *transformPtr;

				if (child.layout.alignment & Alignment::Left)
				{
					transform.transform.position.x = 0.0f;
				}
				else if (child.layout.alignment & Alignment::Center)
				{
					transform.transform.position.x = (selfData.size.x - child.size.x) * 0.5f;
				}
				else if (child.layout.alignment & Alignment::Right)
				{
					transform.transform.position.x = selfData.size.x - child.size.x;
				}
				transform.transform.position.y = heightCount - child.size.y;
			}

			for (auto& child : children)
			{
				child.callback(child);
			}
		};

		context.addChild(id, size, layout, callback);
	}

	void Column::operator>>(std::function<void()> func)
	{
		this->operator()(func);
	}

	void Row::operator()(std::function<void()> func)
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();

		auto id = context.getNextId();
		auto parentId = context.getCurrentParent();

		TransformCom trans{};
		TransformAttachTo(trans, parentId);

		ControlCom ctrl{};
		ControlAttachTo(ctrl, parentId);

		ViewPortCom vp{};
		ViewPortAttachTo(vp, parentId);

		world.createUnit(id, trans, ctrl, vp);

		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		Vec2 size{};
		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;

		auto callback = [func](ChildData& selfData)
			{
				auto& world = GetContextStack().getWorld();

				ContextStackGuard guard(selfData.childId);

				func();

				auto& children = GetContextStack().getCurrentChildren();

				float totalWeightWidth = 0.0f;
				float totalPixelOrAutoWidth = 0.0f;

				for (const auto& child : children)
				{
					if (child.layout.widthMode == SizeMode::Weight)
						totalWeightWidth += child.layout.widthValue;
					else
						totalPixelOrAutoWidth += child.size.x;
				}

				float remainingWidth = selfData.size.x - totalPixelOrAutoWidth;
				float widthCount = 0.0f;

				for (auto& child : children)
				{
					// 主轴宽度：权重分配剩余宽度
					if (child.layout.widthMode == SizeMode::Weight)
						child.size.x = (child.layout.widthValue / totalWeightWidth) * remainingWidth;
					// 副轴高度：权重直接乘父高度
					if (child.layout.heightMode == SizeMode::Weight)
						child.size.y = child.layout.heightValue * selfData.size.y;

					widthCount += child.size.x;

					auto transformPtr = world.getWaitAdd<TransformCom>(child.childId);
					if (!transformPtr) continue;
					auto& transform = *transformPtr;

					// 垂直对齐
					if (child.layout.alignment & Alignment::Top)
						transform.transform.position.y = 0.0f;
					else if (child.layout.alignment & Alignment::Middle)
						transform.transform.position.y = (selfData.size.y - child.size.y) * 0.5f;
					else if (child.layout.alignment & Alignment::Bottom)
						transform.transform.position.y = selfData.size.y - child.size.y;

					// 水平位置：从左到右累加
					transform.transform.position.x = widthCount - child.size.x;
				}

				for (auto& child : children)
				{
					child.callback(child);
				}
			};

		context.addChild(id, size, layout, callback);
	}

	void Row::operator>>(std::function<void()> func)
	{
		this->operator()(func);
	}

	void Spacer::operator()()
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();
		auto id = context.getNextId(true);
		auto parentId = context.getCurrentParent();
		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		Vec2 size{};
		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;

		context.addChild(id, size, layout, [](ChildData& selfData) {});
	}

	void Box::operator()(std::function<void()> func)
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();

		auto id = context.getNextId();
		auto parentId = context.getCurrentParent();

		TransformCom trans{};
		TransformAttachTo(trans, parentId);

		ControlCom ctrl{};
		ControlAttachTo(ctrl, parentId);

		ViewPortCom vp{};
		ViewPortAttachTo(vp, parentId);

		world.createUnit(id, trans, ctrl, vp);

		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		Vec2 size{};
		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;

		auto callback = [func](ChildData& selfData)
			{
				auto& world = GetContextStack().getWorld();

				ContextStackGuard guard(selfData.childId);

				func();

				auto& children = GetContextStack().getCurrentChildren();

				// 计算所有子元素的尺寸
				for (auto& child : children)
				{
					// 宽度：若为 Weight，则按父宽度比例计算
					if (child.layout.widthMode == SizeMode::Weight)
					{
						child.size.x = child.layout.widthValue * selfData.size.x;
					}
					// 若为 Pixels 或 Auto，保留 addChild 时传入的尺寸

					// 高度：若为 Weight，则按父高度比例计算
					if (child.layout.heightMode == SizeMode::Weight)
					{
						child.size.y = child.layout.heightValue * selfData.size.y;
					}
				}

				// 设置每个子元素的位置（堆叠在同一区域）
				for (auto& child : children)
				{
					auto transformPtr = world.getWaitAdd<TransformCom>(child.childId);
					if (!transformPtr) continue;
					auto& transform = *transformPtr;

					// 水平位置（根据子元素 alignment）
					if (child.layout.alignment & Alignment::Left)
					{
						transform.transform.position.x = 0.0f;
					}
					else if (child.layout.alignment & Alignment::Center)
					{
						transform.transform.position.x = (selfData.size.x - child.size.x) * 0.5f;
					}
					else if (child.layout.alignment & Alignment::Right)
					{
						transform.transform.position.x = selfData.size.x - child.size.x;
					}
					else
					{
						transform.transform.position.x = 0.0f; // 默认左对齐
					}

					// 垂直位置（根据子元素 alignment）
					if (child.layout.alignment & Alignment::Top)
					{
						transform.transform.position.y = 0.0f;
					}
					else if (child.layout.alignment & Alignment::Middle)
					{
						transform.transform.position.y = (selfData.size.y - child.size.y) * 0.5f;
					}
					else if (child.layout.alignment & Alignment::Bottom)
					{
						transform.transform.position.y = selfData.size.y - child.size.y;
					}
					else
					{
						transform.transform.position.y = 0.0f; // 默认顶部对齐
					}
				}

				// 递归调用所有子元素的回调
				for (auto& child : children)
				{
					child.callback(child);
				}
			};

		context.addChild(id, size, layout, callback);
	}

	void Box::operator>>(std::function<void()> func)
	{
		this->operator()(func);
	}
	
	void Button::operator()()
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();

		auto id = context.getNextId();
		auto parentId = context.getCurrentParent();

		TransformCom trans{};
		TransformAttachTo(trans, parentId);

		ControlCom ctrl{};
		ControlAttachTo(ctrl, parentId);

		ViewPortCom vp{};
		ViewPortAttachTo(vp, parentId);

		ButtonLogic logic{};
		FillButtonVisual fillVisual
		{
			.fillColor = color,
			.borderColor = borderColor,
			.textColor = textColor,
			.font = font,
			.fontSize = fontSize,
			.spacing = spacing,
			.text = text,
			.roundness = roundness
		};

		auto depth = context.depth();

		ButtonMessage msg = { .priority = static_cast<ClickPriorityT>(ClickPriority::MaxValue - depth) };//UI层级越高depth越大，点击优先级越高，对应数值越低
		ButtonCallback callbackCom{ .onPress = onPress, .onClick = onClick, .onCancel = onCancel };

		LayerCom layerCom{ .layer = context.getWorld().getUiLayer(), .priority = static_cast<core::DrawPriorityT>(context.getBeginDrawPriority() + depth)};

		world.createUnit(id, trans, ctrl, vp, logic, fillVisual, callbackCom, msg, layerCom);
		Vec2 size{};

		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;

		if (widthMode == SizeMode::Auto || heightMode == SizeMode::Auto)
		{
			auto textSize = utils::MeasureTextSize(font, text, fontSize, spacing);
			if (widthMode == SizeMode::Auto) size.x = textSize.x + 20.0f;
			if (heightMode == SizeMode::Auto) size.y = textSize.y + 10.0f;
		}

		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		auto callback = [](ChildData& selfData)
			{
				auto& world = GetContextStack().getWorld();
				auto& ctrl = *world.getWaitAdd<ControlCom>(selfData.childId);
				ctrl.interactiveArea = Rect{ 0.0f, 0.0f, selfData.size.x, selfData.size.y };
			};

		context.addChild(id, size, layout, callback);
	}

	void Text::operator()()
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();

		auto id = context.getNextId();
		auto parentId = context.getCurrentParent();

		TransformCom trans{};
		TransformAttachTo(trans, parentId);

		ControlCom ctrl{};
		ControlAttachTo(ctrl, parentId);

		ViewPortCom vp{};
		ViewPortAttachTo(vp, parentId);

		TextBox textBox
		{
			.text = text,
			.textSize = textSize,
			.spacing = spacing,
			.font = font,
			.textColor = textColor
		};

		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		LayerCom layerCom{ .layer = context.getWorld().getUiLayer(), .priority = static_cast<core::DrawPriorityT>(context.getBeginDrawPriority() + context.depth()) };

		world.createUnit(id, trans, ctrl, vp, textBox, layerCom);

		Vec2 size{};

		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;
		if (widthMode == SizeMode::Auto || heightMode == SizeMode::Auto)
		{
			auto size_ = utils::MeasureTextSize(font, text, textSize, spacing);
			if (widthMode == SizeMode::Auto) size.x = size_.x;
			if (heightMode == SizeMode::Auto) size.y = size_.y;
		}

		auto callback = [](ChildData& selfData)
			{
				auto& world = GetContextStack().getWorld();
				auto& ctrl = *world.getWaitAdd<ControlCom>(selfData.childId);
				ctrl.interactiveArea = Rect{ 0.0f, 0.0f, selfData.size.x, selfData.size.y };
			};

		context.addChild(id, size, layout, callback);
	};

	//反色按钮：复用 ButtonLogic / ButtonOnClick / LayerCom，仅将绘制组件替换为 InvertButtonVisual
	void InvertButton::operator()()
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();

		auto id = context.getNextId();
		auto parentId = context.getCurrentParent();

		TransformCom trans{};
		TransformAttachTo(trans, parentId);

		ControlCom ctrl{};
		ControlAttachTo(ctrl, parentId);

		ViewPortCom vp{};
		ViewPortAttachTo(vp, parentId);

		ButtonLogic logic{};//复用既有交互状态机，保证悬停/按下判定一致
		InvertButtonVisual invertVisual
		{
			.deepColor = deepColor,
			.midColor = midColor,
			.lightColor = lightColor,
			.theme = theme,
			.font = font,
			.fontSize = fontSize,
			.spacing = spacing,
			.text = text,
			.roundness = roundness
		};

		auto depth = context.depth();

		ButtonMessage msg = { .priority = static_cast<ClickPriorityT>(ClickPriority::MaxValue - depth) };
		ButtonCallback callbackCom{ .onPress = onPress, .onClick = onClick, .onCancel = onCancel };

		LayerCom layerCom{ .layer = context.getWorld().getUiLayer(), .priority = static_cast<core::DrawPriorityT>(context.getBeginDrawPriority() + depth) };

		world.createUnit(id, trans, ctrl, vp, logic, invertVisual, callbackCom, msg, layerCom);
		Vec2 size{};

		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;

		if (widthMode == SizeMode::Auto || heightMode == SizeMode::Auto)
		{
			auto textSize = utils::MeasureTextSize(font, text, fontSize, spacing);
			if (widthMode == SizeMode::Auto) size.x = textSize.x + 20.0f;
			if (heightMode == SizeMode::Auto) size.y = textSize.y + 10.0f;
		}

		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		auto callback = [](ChildData& selfData)
			{
				auto& world = GetContextStack().getWorld();
				auto& ctrl = *world.getWaitAdd<ControlCom>(selfData.childId);
				ctrl.interactiveArea = Rect{ 0.0f, 0.0f, selfData.size.x, selfData.size.y };
			};

		context.addChild(id, size, layout, callback);
	}

	//图片按钮：复用 ButtonLogic / ButtonOnClick / LayerCom，仅将绘制组件替换为 ImageButtonVisual（三图直切，无渐变）
	void ImageButton::operator()()
	{
		auto& context = GetContextStack();
		auto& world = context.getWorld();

		auto id = context.getNextId();
		auto parentId = context.getCurrentParent();

		TransformCom trans{};
		TransformAttachTo(trans, parentId);

		ControlCom ctrl{};
		ControlAttachTo(ctrl, parentId);

		ViewPortCom vp{};
		ViewPortAttachTo(vp, parentId);

		ButtonLogic logic{};//复用既有交互状态机，保证悬停/按下判定一致
		ImageButtonVisual imageVisual
		{
			.normalIcon = normalIcon,
			.hoverIcon = hoverIcon,
			.pressedIcon = pressedIcon,
			.fitMode = fitMode,
			.tint = tint
		};

		auto depth = context.depth();

		ButtonMessage msg = { .priority = static_cast<ClickPriorityT>(ClickPriority::MaxValue - depth) };
		ButtonCallback callbackCom{ .onPress = onPress, .onClick = onClick, .onCancel = onCancel };

		LayerCom layerCom{ .layer = context.getWorld().getUiLayer(), .priority = static_cast<core::DrawPriorityT>(context.getBeginDrawPriority() + depth) };

		world.createUnit(id, trans, ctrl, vp, logic, imageVisual, callbackCom, msg, layerCom);
		Vec2 size{};

		if (widthMode == SizeMode::Pixels) size.x = widthValue;
		if (heightMode == SizeMode::Pixels) size.y = heightValue;

		//Auto 尺寸：按常态图片的自然尺寸测量
		if (widthMode == SizeMode::Auto || heightMode == SizeMode::Auto)
		{
			if (normalIcon.valid())
			{
				if (widthMode == SizeMode::Auto) size.x = static_cast<float>(normalIcon.width());
				if (heightMode == SizeMode::Auto) size.y = static_cast<float>(normalIcon.height());
			}
		}

		UILayout layout
		{
			.widthMode = widthMode,
			.widthValue = widthValue,
			.heightMode = heightMode,
			.heightValue = heightValue,
			.alignment = alignment
		};

		auto callback = [](ChildData& selfData)
			{
				auto& world = GetContextStack().getWorld();
				auto& ctrl = *world.getWaitAdd<ControlCom>(selfData.childId);
				ctrl.interactiveArea = Rect{ 0.0f, 0.0f, selfData.size.x, selfData.size.y };
			};

		context.addChild(id, size, layout, callback);
	}

}