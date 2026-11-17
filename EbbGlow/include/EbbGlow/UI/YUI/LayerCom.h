#pragma once

#include <EbbGlow/Core/World.h>

namespace ebbglow::ui::yui
{
	struct LayerCom
	{
		core::Layer* layer;
		core::DrawPriorityT priority;
	};
}