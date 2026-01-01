#pragma once

/*
#include <fstream>
#include <future>

#include <core/ECS.h>
#include <core/World.h>
#include "VisualNovel.h"

namespace visualnovel
{
	

	

	

	struct TextSceneCom
	{
		core::entity txtBoxId;
		rsc::SharedFile::Iterator nextScene;
		ScriptLoader::SceneView view;
	};

	class TextSceneSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<TextSceneCom>* coms;
		core::DoubleComs<ui::ButtonExCom>* buttonComs;
		core::DoubleComs<StandardTextBox>* textBoxComs;
		ScriptLoader* scLoader;
		std::string nextSceneName;
		bool clicked = false;

		core::MessageTypeId pressMsgId;

	public:
		TextSceneSystem(core::World2D& world, ScriptLoader& scLoader) :
			world(&world), coms(world.getDoubleBuffer<TextSceneCom>()), buttonComs(world.getDoubleBuffer<ui::ButtonExCom>()),
			clicked(false), textBoxComs(world.getDoubleBuffer<StandardTextBox>()), scLoader(&scLoader),
			pressMsgId(world.getMessageManager()->getMessageTypeManager().getId<ui::ButtonPressMsg>()){}

		void update() override
		{
			bool processed = false;
			coms->active()->forEach([this, &processed](core::entity id, TextSceneCom& comActive)
			{
				if (clicked)
				{
					buttonComs->active()->forEach([this, &processed](core::entity id, ui::ButtonExCom& comActive)
					{
						if (!processed)
						{
							auto msgs = world->getMessageManager()->getMessageList(id);
							for (auto& msg : *msgs)
							{
								if (msg->getType() == pressMsgId)
								{
									processed = true;
									break;
								}
							}
						}
					});
					if (!processed)
					{
						if (textBoxComs->active()->get(comActive.txtBoxId)->activePixel >= textBoxComs->active()->get(comActive.txtBoxId)->totalPixel)
						{
							scLoader->addLog(comActive.view);
							scLoader->loadScene(comActive.nextScene);
						}
						else
						{
							world->getSystem<vn::StandardTextBoxSystem>()->skip(comActive.txtBoxId);
						}
					}
					clicked = false;
				}
				else
				{
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyPressed(KEY_SPACE) || GetMouseWheelMove() < 0.0f || IsTouchPress())
					{
						clicked = true;
					}
				}
			});
		}
	};

	struct SelectSceneCom
	{
		std::vector<core::entity> buttonIdList;
		std::vector<rsc::SharedFile::Iterator> nextSceneList;
	};

	class SelectSceneSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<SelectSceneCom>* coms;
		ScriptLoader* scLoader;

		core::MessageTypeId pressMsgId = world->getMessageManager()->getMessageTypeManager().getId<ui::ButtonReleaseMsg>();

	public:
		SelectSceneSystem(core::World2D& world, ScriptLoader& scLoader) :
			world(&world), coms(world.getDoubleBuffer<SelectSceneCom>()), scLoader(&scLoader) {}

		void update() override
		{
			coms->active()->forEach([this](core::entity id, SelectSceneCom& comActive)
			{
				for (size_t i = 0; i < comActive.buttonIdList.size(); ++i)
				{
					auto msgs = world->getMessageManager()->getMessageList(comActive.buttonIdList[i]);
					for (auto& msg : *msgs)
					{
						if (msg->getType() == pressMsgId)
						{
							scLoader->loadScene(comActive.nextSceneList[i]);
							return;
						}
					}
				}
			});
		}
	};

	struct DelaySceneCom
	{
		double delay;
		double timeCount = 0;

		rsc::SharedFile::Iterator targetScene;
	};

	class DelaySceneSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<DelaySceneCom>* coms;

		ScriptLoader* scLoader;

	public:
		DelaySceneSystem(core::World2D& world, ScriptLoader& scLoader) : world(&world), coms(world.getDoubleBuffer<DelaySceneCom>()), scLoader(&scLoader) {}

		void update()
		{
			coms->active()->forEach([this](core::entity id, DelaySceneCom& activeCom)
			{
				auto& inactiveCom = *coms->inactive()->get(id);
				if (activeCom.timeCount > activeCom.delay)
				{
					scLoader->loadScene(activeCom.targetScene);
				}
				inactiveCom = activeCom;
				inactiveCom.timeCount += GetFrameTime() * 1000;//To ms
			});
		}
	};

	inline void ScriptLoader::loadScene(rsc::SharedFile::Iterator nextScene)
	{
		for (auto id : idList)
		{
			world.deleteUnit(id);
		}
		idList.clear();
		for (auto id : exIdList)
		{
			world.deleteUnit(id);
		}
		exIdList.clear();
		std::string sceneName;
		std::string sceneType;
		std::string textTmp;
		std::vector<std::string> argsList;
		LoadSceneInfo(nextScene, sceneName, sceneType, argsList);
		while (memcmp(nextScene.get(), "Scene", 5) && !nextScene.eof())
		{
			LoadFunction(nextScene, sceneName, sceneType, textTmp, argsList);
		}

		if (sceneType == "TextScene")
		{
			idList.push_back(world.getEntityManager()->getId());
			auto it = viewer.find(argsList.back());
			SceneView view{ sceneName, SceneView::SceneType::TYPE_TEXTSCENE, textTmp };
			world.createUnit(idList.back(), visualnovel::TextSceneCom{ exIdList.back(), viewer[argsList.back()], view});
		}
		else if (sceneType == "SelectScene")
		{
			idList.push_back(world.getEntityManager()->getId());
			std::vector<rsc::SharedFile::Iterator> targetItList;
			for (auto target : argsList)
			{
				auto it = viewer.find(target);
				targetItList.push_back(it->second);
			}
			world.createUnit(idList.back(), visualnovel::SelectSceneCom{ exIdList, targetItList });
		}
		else if (sceneType == "DelayScene")
		{
			idList.push_back(world.getEntityManager()->getId());
			world.createUnit(idList.back(), visualnovel::DelaySceneCom{ std::stod(argsList[1]), 0, viewer[argsList[0]]});
		}
	}

	inline void ApplyScriptLoader(core::World2D& world, ScriptLoader& scriptLoader, VisualNovelConfig& cfg)
	{
		ui::ApplyButtonEx(world);
		ui::ApplyImageBoxEx(world);
		ui::ApplyTextBoxEx(world);
		world.addPool<StandardTextBox>();
		world.addSystem(vn::StandardTextBoxSystem(world.getDoubleBuffer<StandardTextBox>(), world.getUiLayer(), cfg.LayerDefine.textBoxLayer, cfg));
		world.addPool<visualnovel::TextSceneCom>();
		world.addSystem(TextSceneSystem(world, scriptLoader));
		world.addPool<visualnovel::SelectSceneCom>();
		world.addSystem(SelectSceneSystem(world, scriptLoader));
		world.addPool<DelaySceneCom>();
		world.addSystem(DelaySceneSystem(world, scriptLoader));
		world.addSystem(MusicManager(cfg));
		scriptLoader.mscMgr = world.getSystem<MusicManager>();
	}
}*/