#pragma once

#include <EbbGlow/UI/YUI/Transform.h>
#include <EbbGlow/UI/YUI/Control.h>
#include <EbbGlow/UI/YUI/View.h>
#include <EbbGlow/UI/YUI/LayerCom.h>

#include <EbbGlow/UI/YUI/Click/ClickDispatch.h>

#include <EbbGlow/UI/YUI/ImageBox/ImageBox.h>
#include <EbbGlow/UI/YUI/Button/Button.h>
#include <EbbGlow/UI/YUI/TextBox/TextBox.h>
#include <EbbGlow/UI/YUI/ScrollContainer/ScrollContainer.h>

#include <EbbGlow/UI/YUI/Layout.h>

namespace ebbglow::ui::yui
{
	void ApplyYUI(core::World2D& world);
}