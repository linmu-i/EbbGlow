#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Graphics/Graphics.h>

namespace ebbglow::ui
{
	struct AnimationCom
	{
		std::vector<rsc::SharedTexture2D> framesSequence;
		std::vector<float> frameTime;
		size_t activeFrame;
		float frameTimeCount;

		bool loop;
		bool stop;

		Vec2 position;

		//core::Layers* layers;
		//uint8_t layerDepth;
		core::Layer* layer;

		AnimationCom(std::vector<std::string> framesPath, float totalPlayTime, Vec2 position, core::Layers* layers, size_t layerDepth);
		AnimationCom(std::vector<std::string> framesPath, float totalPlayTime, Vec2 position, core::Layer* layer);
	};

	class AnimationDraw : public core::DrawBase
	{
	private:
		rsc::SharedTexture2D frame;
		Vec2 pos;

	public:
		AnimationDraw(const rsc::SharedTexture2D& frame, const Vec2& pos) : frame(frame), pos(pos) {}

		void draw() override
		{
			gfx::DrawTexture(frame, pos);
		}
	};

	class AnimationSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<AnimationCom>& animations;

	public:
		AnimationSystem(core::DoubleComs<AnimationCom>* animations) : animations(*animations) {}

		AnimationSystem& stop(core::entity id);
		AnimationSystem& play(core::entity id);
		AnimationSystem& loop(core::entity id, bool state);

		void update() override;
	};

	inline void ApplyAnimation(core::World2D& world)
	{
		world.addPool<AnimationCom>();
		world.addSystem(AnimationSystem(world.getDoubleBuffer<AnimationCom>()));
	}
}