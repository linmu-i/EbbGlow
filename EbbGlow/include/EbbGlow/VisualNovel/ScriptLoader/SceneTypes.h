#pragma once

#include <EbbGlow/UI/UI.h>
#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>


namespace ebbglow::visualnovel
{
	struct TextSceneCom {};

	class TextSceneSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<TextSceneCom>* coms;
		core::DoubleComs<ui::ButtonExCom>* buttonComs;
		core::DoubleComs<MainTextBoxCom>* textBoxComs;
		ScriptLoader* scLoader;
		std::string nextSceneName;
		bool clicked = false;

		core::MessageTypeId pressMsgId;

	public:
		TextSceneSystem(core::World2D& world, ScriptLoader& scLoader) :
			world(&world), coms(world.getDoubleBuffer<TextSceneCom>()), buttonComs(world.getDoubleBuffer<ui::ButtonExCom>()),
			clicked(false), textBoxComs(world.getDoubleBuffer<MainTextBoxCom>()), scLoader(&scLoader),
			pressMsgId(world.getMessageManager()->getMessageTypeManager().getId<ui::ButtonPressMsg>()) {
		}

		void update() override;
	};

	void SceneType_TextScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept;

	inline void ApplyTextScene(core::World2D& world, ScriptLoader& scLoader)
	{
		world.addPool<TextSceneCom>();
		world.addSystem(TextSceneSystem(world, scLoader));
	}

	struct SelectSceneCom {};

	class SelectSceneSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<SelectSceneCom>* coms;
		ScriptLoader* scLoader;

		core::MessageTypeId releaseMsgId = world->getMessageManager()->getMessageTypeManager().getId<ui::ButtonReleaseMsg>();

	public:
		SelectSceneSystem(core::World2D& world, ScriptLoader& scLoader) :
			world(&world), coms(world.getDoubleBuffer<SelectSceneCom>()), scLoader(&scLoader) {
		}

		void update() override;
	};

	void SceneType_SelectScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept;

	inline void ApplySelectScene(core::World2D& world, ScriptLoader& scLoader)
	{
		world.addPool<SelectSceneCom>();
		world.addSystem(SelectSceneSystem(world, scLoader));
	}



	struct DelaySceneCom
	{
		float delay;
		float timeCount;

		DelaySceneCom() : delay(0.0f), timeCount(0.0f) {}
		DelaySceneCom(float delay) : delay(delay), timeCount(0.0f) {}
		DelaySceneCom(float delay, float timeCount) : delay(delay), timeCount(timeCount) {}
	};

	class DelaySceneSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<DelaySceneCom>* coms;

		ScriptLoader* scLoader;

	public:
		DelaySceneSystem(core::World2D& world, ScriptLoader& scLoader) : world(&world), coms(world.getDoubleBuffer<DelaySceneCom>()), scLoader(&scLoader) {}

		void update() override;
	};

	void SceneType_DelayScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept;

	inline void ApplyDelayScene(core::World2D& world, ScriptLoader& scLoader)
	{
		world.addPool<DelaySceneCom>();
		world.addSystem(DelaySceneSystem(world, scLoader));
	}
}