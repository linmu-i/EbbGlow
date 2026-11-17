#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/YUIBasic.h>

namespace ebbglow::ui::yui
{
	struct ScrollContainer
	{
		core::entity panelId;

		enum class ScrollDirection
		{
			Vertical,
			Horizontal,
		} direction = ScrollDirection::Vertical;

		float height = 0;
		float maxOffsetY = 0;
		float minOffsetY = 0;
		float offsetY = 0;

		float width = 0;
		float maxOffsetX = 0;
		float minOffsetX = 0;
		float offsetX = 0;

		float speed = 1.0;
	};

	class ScrollContainerSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ScrollContainer>* scPool;
		core::DoubleComs<TransformCom>* transPool;
		core::DoubleComs<ControlCom>* controlPool;

	public:
		ScrollContainerSystem(core::DoubleComs<ScrollContainer>* scPool, core::DoubleComs<TransformCom>* transPool, core::DoubleComs<ControlCom>* controlPool)
			: scPool(scPool), transPool(transPool), controlPool(controlPool) {}
		ScrollContainerSystem(core::World2D& world)
			: scPool(world.getDoubleBuffer<ScrollContainer>()), transPool(world.getDoubleBuffer<TransformCom>()), controlPool(world.getDoubleBuffer<ControlCom>()) {}

		void update() override;
	};

	void ApplyScrollContainer(core::World2D& world);
}