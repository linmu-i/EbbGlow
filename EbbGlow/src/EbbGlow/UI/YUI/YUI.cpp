#include <EbbGlow/UI/YUI/YUI.h>

namespace ebbglow::ui::yui
{
	void ApplyYUI(core::World2D& world)
	{
		world.addPool<TransformCom>();
		world.addPool<ControlCom>();
		world.addPool<ViewPortCom>();
		world.addPool<LayerCom>();

		ApplyClickDispatch(world);
		ApplyImageBox(world);
		ApplyButton(world);
		ApplyTextBox(world);
		ApplyScrollContainer(world);
	}
}