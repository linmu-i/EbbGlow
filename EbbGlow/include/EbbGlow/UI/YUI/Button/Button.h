#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/Button/ButtonLogic.h>
#include <EbbGlow/UI/YUI/Button/FillButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/InvertButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/ImageButtonVisual.h>
#include <EbbGlow/UI/YUI/Button/ButtonMessage.h>
#include <EbbGlow/UI/YUI/Button/ButtonCallback.h>

namespace ebbglow::ui::yui
{
	inline void ApplyButton(core::World2D& world)
	{
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonPressedMessage>();
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonClickedMessage>();
		world.getMessageManager()->getMessageTypeManager().registeredType<ButtonCancelledMessage>();

		world.addPool<ButtonLogic>();
		world.addPool<FillButtonVisual>();
		world.addPool<InvertButtonVisual>();
		world.addPool<ImageButtonVisual>();
		world.addPool<ButtonMessage>();
		world.addPool<ButtonCallback>();

		world.addSystem(ButtonLogicSystem(world));
		world.addSystem(FillButtonVisualSystem(world));
		world.addSystem(InvertButtonVisualSystem(world));
		world.addSystem(ImageButtonVisualSystem(world));
		world.addSystem(ButtonMessageSystem(world));
		world.addSystem(ButtonCallbackSystem(world));
	}
}