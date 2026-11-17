#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/YUIBasic.h>

namespace ebbglow::ui::yui
{
	struct ButtonLogic
	{
		bool isPressed = false;
		bool isDown = false;
		bool isHovered = false;
		bool isClicked = false;
		bool isCancelled = false;
	};

	class ButtonLogicSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ButtonLogic>* buttonLogics;
		core::DoubleComs<TransformCom>* transforms;
		core::DoubleComs<ControlCom>* controls;

	public:
		ButtonLogicSystem(core::DoubleComs<ButtonLogic>* buttonLogics, core::DoubleComs<TransformCom>* transforms, core::DoubleComs<ControlCom>* controls)
			: buttonLogics(buttonLogics), transforms(transforms), controls(controls) {
		}

		ButtonLogicSystem(core::World2D& world)
			: buttonLogics(world.getDoubleBuffer<ButtonLogic>()), transforms(world.getDoubleBuffer<TransformCom>()), controls(world.getDoubleBuffer<ControlCom>()) {
		}

		void update() override;
	};
}