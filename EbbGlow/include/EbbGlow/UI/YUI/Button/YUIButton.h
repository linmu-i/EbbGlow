#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/Button/YUIButtonLogic.h>
#include <EbbGlow/UI/YUI/Button/YUIFillButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/YUIInvertButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/YUIImageButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/YUIButtonMessage.h>
#include <EbbGlow/UI/YUI/Button/YUIButtonOnClick.h>

namespace ebbglow::ui::yui
{
	inline void ApplyButton(core::World2D& world)
	{
		world.addPool<ButtonLogic>();
		world.addPool<FillButtonVisual>();
		world.addPool<InvertButtonVisual>();
		world.addPool<ImageButtonVisual>();
		world.addPool<ButtonMessage>();
		world.addPool<ButtonOnClick>();

		world.addSystem(ButtonLogicSystem(world));
		world.addSystem(FillButtonVisualSystem(world));
		world.addSystem(InvertButtonVisualSystem(world));
		world.addSystem(ImageButtonVisualSystem(world));
		world.addSystem(ButtonMessageSystem(world));
		world.addSystem(ButtonOnClickSystem(world));
	}
}