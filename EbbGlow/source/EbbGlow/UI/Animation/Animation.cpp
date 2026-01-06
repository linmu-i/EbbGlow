#include <EbbGlow/UI/Animation/Animation.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::ui
{
	AnimationCom::AnimationCom(std::vector<std::string> framesPath, float totalPlayTime, Vec2 position, core::Layers* layers, size_t layerDepth) :
		layer(&(*layers)[layerDepth]), loop(false), stop(true), frameTimeCount(0.0f), activeFrame(0), position(position)
	{
		frameTime.clear();
		framesSequence.clear();
		float time = totalPlayTime / framesPath.size();
		for (int i = 0; i < framesPath.size(); ++i)
		{
			frameTime.push_back(time);
			framesSequence.push_back(framesPath[i].c_str());
		}
	}

	AnimationCom::AnimationCom(std::vector<std::string> framesPath, float totalPlayTime, Vec2 position, core::Layer* layer) :
		layer(layer), loop(false), stop(true), frameTimeCount(0.0f), activeFrame(0), position(position)
	{
		frameTime.clear();
		framesSequence.clear();
		float time = totalPlayTime / framesPath.size();
		for (int i = 0; i < framesPath.size(); ++i)
		{
			frameTime.push_back(time);
			framesSequence.push_back(framesPath[i].c_str());
		}
	}

	AnimationSystem& AnimationSystem::stop(core::entity id)
	{
		if (auto active = animations.active()->get(id) != nullptr)
		{
			animations.active()->get(id)->stop = true;
		}
		if (auto active = animations.inactive()->get(id) != nullptr)
		{
			animations.inactive()->get(id)->stop = true;
		}
		return *this;
	}

	AnimationSystem& AnimationSystem::play(core::entity id)
	{
		if (auto active = animations.active()->get(id) != nullptr)
		{
			animations.active()->get(id)->stop = false;
		}
		if (auto active = animations.inactive()->get(id) != nullptr)
		{
			animations.inactive()->get(id)->stop = false;
		}
		return *this;
	}

	AnimationSystem& AnimationSystem::loop(core::entity id, bool state)
	{
		if (auto active = animations.active()->get(id) != nullptr)
		{
			animations.active()->get(id)->loop = state;
		}
		if (auto active = animations.inactive()->get(id) != nullptr)
		{
			animations.inactive()->get(id)->loop = state;
		}
		return *this;
	}

	void AnimationSystem::update()
	{
		animations.active()->forEach
		(
			[this](core::entity id, AnimationCom& animationActive)
			{
				AnimationCom& animationInactive = *(animations.inactive()->get(id));
				if (!animationInactive.stop)
				{

					//播放当前帧
					(*animationActive.layer).push_back(std::make_unique<AnimationDraw>(AnimationDraw(animationActive.framesSequence[animationActive.activeFrame], animationActive.position)));

					animationActive.frameTimeCount += GetFrameTime();
					animationInactive.frameTimeCount += GetFrameTime();

					if (animationInactive.frameTimeCount >= animationInactive.frameTime[animationInactive.activeFrame])//播放时间到达，切换下一帧
					{
						++(animationInactive.activeFrame);
						animationInactive.frameTimeCount = 0.0f;
					}
					if (animationInactive.activeFrame >= animationInactive.framesSequence.size())
					{
						animationInactive.activeFrame = 0;
						if (!animationInactive.loop)//非循环播放，播放完毕后复位活跃帧并停止播放，循环播放则仅复位活跃帧
						{
							animationInactive.stop = true;
							animationActive.stop = true;
						}
					}
				}
			}
		);
	}
}