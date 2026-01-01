#include <EbbGlow/UI/Animation/KeyFrameAnimation.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::ui
{
	void KeyFramesAnimationSystem::update()
	{
		coms.active()->forEach
		(
			[this](core::entity id, KeyFramesAnimationCom& activeCom)
			{

				if (activeCom.keyFrames.size() == 0)
				{
					return;
				}
				auto& inactiveCom = *(coms.inactive()->get(id));

				if (activeCom.activeFrame == 0)
				{
					(*(activeCom.layer))[activeCom.layerDepth].push_back
					(
						std::make_unique<KeyFramesAnimationDraw>
						(
							activeCom.keyFrames[0].position,
							activeCom.keyFrames[0].origin,
							activeCom.keyFrames[0].scale * activeCom.scale,
							activeCom.keyFrames[0].rotation,
							activeCom.keyFrames[0].alpha,
							activeCom.texture
						)
					);
				}
				else
				{
					float deltaScale = activeCom.timeCount / activeCom.keyFrames[activeCom.activeFrame].duration;
					deltaScale = std::clamp(deltaScale, 0.0f, 1.0f);
					(*(activeCom.layer))[activeCom.layerDepth].push_back
					(
						std::make_unique<KeyFramesAnimationDraw>
						(
							activeCom.keyFrames[activeCom.activeFrame - 1].position + (activeCom.keyFrames[activeCom.activeFrame].position - activeCom.keyFrames[activeCom.activeFrame - 1].position) * deltaScale,
							activeCom.keyFrames[activeCom.activeFrame].origin,
							(activeCom.keyFrames[activeCom.activeFrame - 1].scale + (activeCom.keyFrames[activeCom.activeFrame].scale - activeCom.keyFrames[activeCom.activeFrame - 1].scale) * deltaScale) * activeCom.scale,
							activeCom.keyFrames[activeCom.activeFrame - 1].rotation + (activeCom.keyFrames[activeCom.activeFrame].rotation - activeCom.keyFrames[activeCom.activeFrame - 1].rotation) * deltaScale,
							activeCom.keyFrames[activeCom.activeFrame - 1].alpha + (activeCom.keyFrames[activeCom.activeFrame].alpha - activeCom.keyFrames[activeCom.activeFrame - 1].alpha) * deltaScale,
							activeCom.texture
						)
					);
				}
				if (!activeCom.stop)
				{
					inactiveCom.timeCount += GetFrameTime();
				}
				if (activeCom.timeCount > activeCom.keyFrames[activeCom.activeFrame].duration)
				{

					if (inactiveCom.activeFrame >= activeCom.keyFrames.size() - 1)
					{
						if (!activeCom.loop)
						{
							inactiveCom.stop = true;
							inactiveCom.activeFrame = activeCom.keyFrames.size() - 1;
						}
						else
						{
							inactiveCom.timeCount = 0.0f;
							inactiveCom.activeFrame = 0;
						}
					}
					else
					{
						inactiveCom.timeCount = 0.0f;
						++inactiveCom.activeFrame;
					}
				}
			}
		);
	}

	void KeyFramesAnimationSystem::play(core::entity id)
	{
		auto active = coms.active()->get(id);
		auto inactive = coms.inactive()->get(id);
		if (active)
		{
			active->stop = false;
			inactive->stop = false;
		}
	}

	void KeyFramesAnimationSystem::loop(core::entity id, bool isLoop)
	{
		auto active = coms.active()->get(id);
		auto inactive = coms.inactive()->get(id);
		if (active)
		{
			active->loop = isLoop;
			inactive->loop = isLoop;
		}
	}

	void KeyFramesAnimationSystem::pause(core::entity id)
	{
		auto active = coms.active()->get(id);
		auto inactive = coms.inactive()->get(id);
		if (active)
		{
			active->stop = true;
			inactive->stop = true;
		}
	}

	void KeyFramesAnimationSystem::stop(core::entity id)
	{
		auto active = coms.active()->get(id);
		auto inactive = coms.inactive()->get(id);
		if (active)
		{
			active->stop = true;
			active->activeFrame = 0;
			active->timeCount = 0.0f;
			inactive->stop = true;
			inactive->activeFrame = 0;
			inactive->timeCount = 0.0f;
		}
	}
}