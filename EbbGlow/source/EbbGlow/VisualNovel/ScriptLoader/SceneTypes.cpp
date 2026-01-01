#include <EbbGlow/VisualNovel/ScriptLoader/SceneTypes.h>
#include <EbbGlow/Utils/Input.h>
#include <chrono>

namespace ebbglow::visualnovel
{
	void TextSceneSystem::update()
	{
		bool processed = false;
		bool thisFrameClicked = false;
		
		if (!clicked)
		{
			if (input::MousePressed(input::MouseButton::Left) ||
				input::KeyDown(input::Keyboard::LeftControl)  ||
				input::KeyDown(input::Keyboard::LeftControl)  ||
				input::KeyPressed(input::Keyboard::Space)     ||
				input::MouseWheelDelta() < 0
				) thisFrameClicked = true;
			for (auto& p : input::PointList())
			{
				if (input::PointPressed(p.id))
				{
					thisFrameClicked = true;
					break;
				}
			}

		}
		if (thisFrameClicked)
		{
			clicked = true;
		}
		else
		{
			clicked = false;
		}

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
						if (textBoxComs->active()->get(scLoader->exIdList.back())->activePixel >= textBoxComs->active()->get(scLoader->exIdList.back())->totalPixel)
						{
							LogView logTmp = scLoader->logTmp;
							logTmp.text = world->getDoubleBuffer<MainTextBoxCom>()->active()->get(scLoader->exIdList.back())->textStr;
							scLoader->addLog(std::move(logTmp));
							auto next = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), scLoader->sceneView.find(scLoader->sceneArgs.front())->second);
							world->deleteUnit(id);
							world->getEntityManager()->recycleId(id);
							scLoader->loadScene(next);
						}
						else
						{
							world->getSystem<vn::MainTextBoxSystem>()->skip(scLoader->exIdList.back());
						}
					}
					clicked = false;
				}
			});
	}

	void SceneType_TextScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept
	{
		for (auto id : scLoader->idList)
		{
			scLoader->world.deleteUnit(id);
			scLoader->world.getEntityManager()->recycleId(id);
		}
		scLoader->idList.clear();
		for (auto id : scLoader->exIdList)
		{
			scLoader->world.deleteUnit(id);
			scLoader->world.getEntityManager()->recycleId(id);
		}
		scLoader->exIdList.clear();
		auto id = scLoader->world.getEntityManager()->getId();
		scLoader->world.createUnit(id, TextSceneCom());
	}



	void SelectSceneSystem::update()
	{
		coms->active()->forEach([this](core::entity id, SelectSceneCom& comActive)
			{
				for (size_t i = 0; i < scLoader->exIdList.size(); ++i)
				{
					auto msgs = world->getMessageManager()->getMessageList(scLoader->exIdList[i]);
					for (auto& msg : *msgs)
					{
						if (msg->getType() == releaseMsgId)
						{
							auto it = scLoader->sceneView.find(scLoader->sceneArgs[i]);
							if (it == scLoader->sceneView.end()) break;
							auto scIt = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), it->second);
							world->deleteUnit(id);
							world->getEntityManager()->recycleId(id);
							scLoader->loadScene(scIt);
							return;
						}
					}
				}
			});
	}

	void SceneType_SelectScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept
	{
		for (auto id : scLoader->idList)
		{
			scLoader->world.deleteUnit(id);
			scLoader->world.getEntityManager()->recycleId(id);
		}
		scLoader->idList.clear();
		for (auto id : scLoader->exIdList)
		{
			scLoader->world.deleteUnit(id);
			scLoader->world.getEntityManager()->recycleId(id);
		}
		scLoader->exIdList.clear();
		auto id = scLoader->world.getEntityManager()->getId();
		scLoader->world.createUnit(id, SelectSceneCom());
	}



	void DelaySceneSystem::update()
	{
		coms->active()->forEach([this](core::entity id, DelaySceneCom& activeCom)
			{
				auto& inactiveCom = *coms->inactive()->get(id);
				if (activeCom.timeCount > activeCom.delay)
				{
					world->deleteUnit(id);
					world->getEntityManager()->recycleId(id);
					auto it = scLoader->sceneView.find(scLoader->sceneArgs[0]);
					if (it == scLoader->sceneView.end()) return;
					auto scIt = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), it->second);
					scLoader->loadScene(scIt);
				}
				inactiveCom = activeCom;
				inactiveCom.timeCount += utils::GetFrameTime() * 1000;//To ms
			});
	}

	void SceneType_DelayScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept
	{
		for (auto id : scLoader->idList)
		{
			scLoader->world.deleteUnit(id);
			scLoader->world.getEntityManager()->recycleId(id);
		}
		scLoader->idList.clear();
		for (auto id : scLoader->exIdList)
		{
			scLoader->world.deleteUnit(id);
			scLoader->world.getEntityManager()->recycleId(id);
		}
		scLoader->exIdList.clear();
		auto id = scLoader->world.getEntityManager()->getId();
		float delay = 0.0f;
		try
		{
			delay = std::stof(args[1]);
		}
		catch (...)
		{
			delay = 0.0f;
		}
		scLoader->world.createUnit(id, DelaySceneCom(delay));
	}
}