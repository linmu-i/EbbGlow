#pragma once

#include "core/ECS.h"
#include "core/World.h"
#include "utils/Resource.h"

namespace ui
{
	struct AnimationCom
	{
		std::vector<rsc::SharedTexture2D> framesSequence;
		std::vector<float> frameTime;
		size_t activeFrame;
		float frameTimeCount;

		bool loop;
		bool stop;

		Vector2 position;
		
		ecs::Layers* layers;
		uint8_t layerDepth;

		AnimationCom(std::vector<std::string> framesPath, float totalPlayTime, Vector2 position, ecs::Layers* layers, size_t layerDepth) : 
			layers(layers), layerDepth(layerDepth), loop(false), stop(true), frameTimeCount(0.0f), activeFrame(0), position(position)
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
		/*
		AnimationCom& operator=(const AnimationCom& other)
		{
			framesSequence = other.framesSequence;
			frameTime = other.frameTime;
			activeFrame = other.activeFrame;
			frameTimeCount = frameTimeCount;
			loop = other.loop;
			stop = other.stop;
			position
		}*/
	};

	class AnimationDraw : public ecs::DrawBase
	{
	private:
		rsc::SharedTexture2D frame;
		Vector2 pos;

	public:
		AnimationDraw(const rsc::SharedTexture2D& frame, const Vector2& pos) : frame(frame), pos(pos) {}

		void draw() override
		{
			DrawTexture(frame.get(), pos.x, pos.y, WHITE);
		}
	};

	class AnimationSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<AnimationCom>& animations;

	public:
		AnimationSystem(ecs::DoubleComs<AnimationCom>* animations) : animations(*animations) {}

		AnimationSystem& stop(ecs::entity id)
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

		AnimationSystem& play(ecs::entity id)
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

		AnimationSystem& loop(ecs::entity id, bool state)
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

		void update() override
		{
			animations.active()->forEach
			(
				[this](ecs::entity id, AnimationCom& animationActive)
				{
					AnimationCom& animationInactive = *(animations.inactive()->get(id));
					if (!animationInactive.stop)
					{

						//播放当前帧
						(*animationActive.layers)[animationActive.layerDepth].push_back(std::make_unique<AnimationDraw>(AnimationDraw(animationActive.framesSequence[animationActive.activeFrame], animationActive.position)));

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
	};

	void ApplyAnimation(ecs::World2D& world)
	{
		world.addPool<AnimationCom>();
		world.addSystem(AnimationSystem(world.getDoubleBuffer<AnimationCom>()));
	}



	struct KeyFrame
	{
		Vector2 position;
		Vector2 origin;
		float scale;
		float rotation;
		float duration;
		uint8_t alpha;
	};

	struct KeyFramesAnimationCom
	{
		rsc::SharedTexture2D texture;
		std::vector<KeyFrame> keyFrames;
		ecs::Layers* layer;
		int layerDepth;
		int activeFrame;
		float timeCount;
		bool stop;
		bool loop;
	};

	class KeyFramesAnimationDraw : public ecs::DrawBase
	{
	private:
		Vector2 position;
		Vector2 origin;
		float scale;
		float rotation;
		uint8_t alpha;
		rsc::SharedTexture2D texture;

	public:
		KeyFramesAnimationDraw(Vector2 position, Vector2 origin, float scale, float rotation, uint8_t alpha, const rsc::SharedTexture2D& texture) :
			position(position), origin(origin), scale(scale), rotation(rotation), alpha(alpha), texture(texture) {}

		void draw() override
		{
			//DrawTextureEx(texture.get(), position, rotation, scale, { 0xFF,0xFF,0xFF,alpha });
			DrawTexturePro
			(
				texture.get(),
				{ 0, 0, float(texture.get().width), float(texture.get().height) },
				{ position.x, position.y, float(texture.get().width) * scale, float(texture.get().height) * scale },
				origin,
				rotation,
				{ 0xff,0xff,0xff,alpha }
			);
		}
	};

	class KeyFramesAnimationSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<KeyFramesAnimationCom>& coms;

	public:
		KeyFramesAnimationSystem(ecs::DoubleComs<KeyFramesAnimationCom>* coms) : coms(*coms) {}

		void update() override
		{
			coms.active()->forEach
			(
				[this](ecs::entity id, KeyFramesAnimationCom& activeCom)
				{
					if (activeCom.stop)
					{
						return;
					}
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
								activeCom.keyFrames[0].scale,
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
								activeCom.keyFrames[activeCom.activeFrame - 1].scale + (activeCom.keyFrames[activeCom.activeFrame].scale - activeCom.keyFrames[activeCom.activeFrame - 1].scale) * deltaScale,
								activeCom.keyFrames[activeCom.activeFrame - 1].rotation + (activeCom.keyFrames[activeCom.activeFrame].rotation - activeCom.keyFrames[activeCom.activeFrame - 1].rotation) * deltaScale,
								activeCom.keyFrames[activeCom.activeFrame - 1].alpha + (activeCom.keyFrames[activeCom.activeFrame].alpha - activeCom.keyFrames[activeCom.activeFrame - 1].alpha) * deltaScale,
								activeCom.texture
							)
						);
					}
					inactiveCom.timeCount = activeCom.timeCount + GetFrameTime();
					if (inactiveCom.timeCount >= inactiveCom.keyFrames[inactiveCom.activeFrame].duration)
					{
						inactiveCom.timeCount = 0.0f;
						++inactiveCom.activeFrame;
						if (inactiveCom.activeFrame >= inactiveCom.keyFrames.size())
						{
							inactiveCom.activeFrame = 0;
							if (!activeCom.loop)
							{
								inactiveCom.stop = true;
								activeCom.stop = true;
							}
						}
					}
				}
			);
		}

		void play(ecs::entity id)
		{
			auto active = coms.active()->get(id);
			auto inactive = coms.inactive()->get(id);
			if (active)
			{
				active->stop = false;
				inactive->stop = false;
			}
		}

		void loop(ecs::entity id, bool isLoop)
		{
			auto active = coms.active()->get(id);
			auto inactive = coms.inactive()->get(id);
			if (active)
			{
				active->loop = isLoop;
				inactive->loop = isLoop;
			}
		}

		void pause(ecs::entity id)
		{
			auto active = coms.active()->get(id);
			auto inactive = coms.inactive()->get(id);
			if (active)
			{
				active->stop = true;
				inactive->stop = true;
			}
		}

		void stop(ecs::entity id)
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
	};

	void ApplyKeyFramesAnimation(ecs::World2D& world)
	{
		world.addPool<KeyFramesAnimationCom>();
		world.addSystem(KeyFramesAnimationSystem(world.getDoubleBuffer<KeyFramesAnimationCom>()));
	}
}