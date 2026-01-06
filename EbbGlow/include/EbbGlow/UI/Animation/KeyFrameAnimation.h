#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Graphics/Graphics.h>

namespace ebbglow::ui
{
	struct KeyFrame
	{
		Vec2 position;
		Vec2 origin;
		float scale;
		float rotation;
		float duration;
		uint8_t alpha;
	};

	struct KeyFramesAnimationCom
	{
		rsc::SharedTexture2D texture;
		std::vector<KeyFrame> keyFrames;
		core::Layer* layer;
		int activeFrame;
		float timeCount;
		float scale;
		bool stop;
		bool loop;

		KeyFramesAnimationCom() : layer(nullptr), activeFrame(0), timeCount(0.0f), scale(0.0f), stop(false), loop(false) {}
		KeyFramesAnimationCom(rsc::SharedTexture2D texture, std::vector<KeyFrame> keyFrames, core::Layers* layer, float scale, int layerDepth, bool loop) :
			texture(texture), keyFrames(keyFrames), layer(&(*layer)[layerDepth]), loop(loop), activeFrame(0), timeCount(0.0f), stop(false), scale(scale) {
		}
		KeyFramesAnimationCom(rsc::SharedTexture2D texture, core::Layers* layer, float scale, uint8_t layerDepth, bool loop) :
			texture(texture), keyFrames(), layer(&(*layer)[layerDepth]), loop(loop), activeFrame(0), timeCount(0.0f), stop(false), scale(scale) {
		}

		KeyFramesAnimationCom(rsc::SharedTexture2D texture, std::vector<KeyFrame> keyFrames, core::Layer* layer, float scale, bool loop) :
			texture(texture), keyFrames(keyFrames), layer(layer), loop(loop), activeFrame(0), timeCount(0.0f), stop(false), scale(scale) {
		}
	};

	class KeyFramesAnimationDraw : public core::DrawBase
	{
	private:
		Vec2 position;
		Vec2 origin;
		float scale;
		float rotation;
		uint8_t alpha;
		rsc::SharedTexture2D texture;

	public:
		KeyFramesAnimationDraw(Vec2 position, Vec2 origin, float scale, float rotation, uint8_t alpha, const rsc::SharedTexture2D& texture) :
			position(position), origin(origin), scale(scale), rotation(rotation), alpha(alpha), texture(texture) {
		}

		void draw() override
		{
			gfx::DrawTexturePro
			(
				texture,
				{ 0, 0, float(texture.width()), float(texture.height()) },
				{ position.x, position.y, float(texture.width()) * scale, float(texture.height()) * scale },
				origin,
				rotation,
				{ 0xff,0xff,0xff,alpha }
			);
		}
	};

	class KeyFramesAnimationSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<KeyFramesAnimationCom>& coms;

	public:
		KeyFramesAnimationSystem(core::DoubleComs<KeyFramesAnimationCom>* coms) : coms(*coms) {}

		void update() override;

		void play(core::entity id);
		void loop(core::entity id, bool isLoop);
		void pause(core::entity id);
		void stop(core::entity id);
	};

	inline void ApplyKeyFramesAnimation(core::World2D& world)
	{
		world.addPool<KeyFramesAnimationCom>();
		world.addSystem(KeyFramesAnimationSystem(world.getDoubleBuffer<KeyFramesAnimationCom>()));
	}
}