#pragma once

#include <fstream>
#include <sstream>
#include <future>

#include "ECS.h"
#include "World.h"
#include "VisualNovel.h"

namespace visualnovel
{
	class ScriptLoader
	{
	public:
		struct SceneView
		{
			static constexpr uint8_t TYPE_TEXTSCENE = 0;
			static constexpr uint8_t TYPE_SELECTSCENE = 1;
			std::string sceneName;
			uint8_t sceneType;
			std::string text;
		};

	private:
		ecs::World2D& world;
		rlRAII::FileRAII script;
		visualnovel::VisualNovelConfig& cfg;

		std::unordered_map<std::string, rlRAII::FileRAII::Iterator> viewer;
		std::unordered_map<std::string, SceneView> logs;
		std::vector<std::string> targetScene;
		std::vector<ecs::entity> idList;
		std::vector<ecs::entity> exIdList;
		void CreateTextScene(const std::vector<std::string>& languages, ecs::entity textBoxId, rlRAII::Texture2DRAII backGround, bool read)
		{
			world.createUnit(textBoxId, vn::StandardTextBox
			(
				languages[cfg.mainLanguage],
				languages[cfg.secondaryLanguage],
				cfg.textSize, cfg.fontData, cfg.textSpeed, Vector2({ cfg.ScreenWidth * 0.1666667f, cfg.ScreenHeight * 0.75f}), cfg.ScreenWidth * 0.6666667f,
				cfg.showReadText && read ? cfg.readTextColor : WHITE
			));

			float bgScale = std::min(static_cast<float>(cfg.ScreenWidth) / static_cast<float>(backGround.get().width), static_cast<float>(cfg.ScreenHeight) / static_cast<float>(backGround.get().height));
			Vector2 bgPosition;
			if (static_cast<float>(cfg.ScreenWidth) / static_cast<float>(backGround.get().width) > static_cast<float>(cfg.ScreenHeight) / static_cast<float>(backGround.get().height))
			{
				bgPosition = { (cfg.ScreenWidth - backGround.get().width * bgScale) / 2.0f, 0.0f };
			}
			else
			{
				bgPosition = { 0.0f, (cfg.ScreenHeight - backGround.get().height * bgScale) / 2.0f };
			}
			auto idMgr = world.getEntityManager();
			if (backGround.valid())
			{
				idList.push_back(idMgr->getId());
				world.createUnit(idList.back(), ui::ImageBoxExCom({ bgPosition, bgScale, backGround, cfg.backGroundLayer }));
			}
			if (cfg.textBoxBackGround.valid())
			{
				float scale = std::max(float(cfg.ScreenWidth) / cfg.textBoxBackGround.get().width, float(cfg.ScreenHeight * (0.25f + 0.03125f)) / cfg.textBoxBackGround.get().height);
				idList.push_back(idMgr->getId());
				world.createUnit(idList.back(), ui::ImageBoxExCom({ Vector2({ (cfg.ScreenWidth - cfg.textBoxBackGround.get().width * scale) / 2, cfg.ScreenHeight * (0.75f - 0.03125f)}), scale, cfg.textBoxBackGround, cfg.textBoxBackGroundLayer}));
			}
		}
		void CreateNameBox(const std::vector<std::string>& languages, const visualnovel::VisualNovelConfig& cfg, ecs::entity nameBoxId)
		{

		}
		void ReadNextString(std::string& textBuf, rlRAII::FileRAII::Iterator& nextScene)
		{
			textBuf.clear();
			while (*nextScene != '"' && !nextScene.eof()) ++nextScene;
			++nextScene;
			while (*nextScene != '"' && !nextScene.eof())
			{
				if (*nextScene == '\\' && nextScene.position() < nextScene.size() - 1)
				{
					textBuf += nextScene[1];
					nextScene += 2;
				}
				else
				{
					textBuf += *nextScene;
					++nextScene;
				}
			}
			++nextScene;
		};
		void ReadNextNumber(std::string& textBuf, rlRAII::FileRAII::Iterator& nextScene)
		{
			textBuf.clear();
			while ((!isdigit(*nextScene) && *nextScene != '.' && *nextScene != '+' && *nextScene != '-') && !nextScene.eof()) ++nextScene;
			while ((isdigit(*nextScene) || *nextScene == '.' || *nextScene == '+' || *nextScene == '-') && !nextScene.eof())
			{
				textBuf += *nextScene;
				++nextScene;
			}
		}

	public:
		ScriptLoader(ecs::World2D& world, rlRAII::FileRAII script, vn::VisualNovelConfig& cfg) : world(world), script(script), cfg(cfg) {}

		std::future<void> load()
		{
			return std::async(std::launch::async, [this]()
				{
					auto iter = script.begin();

					while (iter)
					{
						if (memcmp(iter.get(), "Scene", 5) == 0 && (iter.position() == 0 || iter[-1] == '\n'))
						{
							std::string stName;
							auto tmpIter = iter;
							iter += 5;
							while (isspace(*iter))
							{
								++iter;
							}
							while (iter && *iter != '(' && !isspace(*iter))
							{
								stName += *iter;
								++iter;
							}
							viewer.emplace(stName, tmpIter);
						}
						++iter;
					}
				});
		}

		void loadScene(rlRAII::FileRAII::Iterator);
	};

	struct TextSceneCom
	{
		ecs::entity txtBoxId;
		rlRAII::FileRAII::Iterator nextScene;
	};

	class TextSceneSystem : public ecs::SystemBase
	{
	private:
		ecs::World2D* world;
		ecs::DoubleComs<TextSceneCom>* coms;
		ecs::DoubleComs<ui::ButtonExCom>* buttonComs;
		ecs::DoubleComs<StandardTextBox>* textBoxComs;
		ScriptLoader* scLoader;
		VisualNovelConfig* cfg;
		std::string nextSceneName;
		bool clicked = false;

	public:
		TextSceneSystem(ecs::World2D& world, VisualNovelConfig& cfg, ScriptLoader& scLoader) :
			world(&world), cfg(&cfg), coms(world.getDoubleBuffer<TextSceneCom>()), buttonComs(world.getDoubleBuffer<ui::ButtonExCom>()),
			clicked(false), textBoxComs(world.getDoubleBuffer<StandardTextBox>()), scLoader(&scLoader) {}

		void update() override
		{
			ecs::MessageTypeId pressMsgId = world->getMessageManager()->getMessageTypeManager().getId<ui::ButtonPressMsg>();
			bool processed = false;
			coms->active()->forEach([this, pressMsgId, &processed](ecs::entity id, TextSceneCom& comActive)
			{
				if (clicked)
				{
					buttonComs->active()->forEach([this, &processed, pressMsgId](ecs::entity id, ui::ButtonExCom& comActive)
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
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						clicked = true;
					}
				}
			});
		}
	};

	inline void ScriptLoader::loadScene(rlRAII::FileRAII::Iterator nextScene)
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
		nextScene += 5;
		while (isspace(*nextScene) && !nextScene.eof()) ++nextScene;
		std::string sceneName;
		std::string sceneType;
		std::vector<std::string> targetList;
		while (*nextScene != '(' && !isspace(*nextScene) && !nextScene.eof())
		{
			sceneName += *nextScene;
			++nextScene;
		}
		++nextScene;
		while (*nextScene != ':' && !nextScene.eof())
		{
			if (!isspace(*nextScene))
			{
				sceneType += *nextScene;
			}
			++nextScene;
		}
		++nextScene;
		std::string targetTmp;
		while (*nextScene != ')' && !nextScene.eof())
		{
			if (!isspace(*nextScene) && *nextScene != ',')
			{
				targetTmp += *nextScene;
			}
			if (*nextScene == ',')
			{
				targetList.push_back(targetTmp);
				targetTmp.clear();
			}
			++nextScene;
		}
		targetList.push_back(targetTmp);
		while (*nextScene != '\n' && !nextScene.eof())	++nextScene;
		++nextScene;
		while (memcmp(nextScene.get(), "Scene", 5) && !nextScene.eof())
		{
			if (!memcmp(nextScene.get(), "TextScene", 9) && (nextScene[9] == '(' || isspace(nextScene[9])))
			{
				std::string textBuf;
				std::vector<std::string> texts;
				nextScene += 10;
				auto LoadText = [&textBuf, &nextScene, this]()
					{
						ReadNextString(textBuf, nextScene);
					};
				LoadText();
				texts.push_back(textBuf);
				LoadText();
				texts.push_back(textBuf);
				LoadText();
				texts.push_back(textBuf);
				LoadText();
				texts.push_back(textBuf);
				LoadText();
				auto readIt = cfg.readTextSet.find(sceneName);
				if (sceneType == "TextScene")
				{
					exIdList.push_back(world.getEntityManager()->getId());
					CreateTextScene(texts, exIdList.back(), rlRAII::Texture2DRAII(textBuf.c_str()), readIt == cfg.readTextSet.end());
				}
				else
				{
					auto idmgr = world.getEntityManager();
					idList.push_back(idmgr->getId());
					CreateTextScene(texts, idList.back(), rlRAII::Texture2DRAII(textBuf.c_str()), readIt == cfg.readTextSet.end());
				}
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "Chr", 3) && (nextScene[3] == '(' || isspace(nextScene[3])))
			{
				std::string nameBuf;
				ReadNextString(nameBuf, nextScene);

				int offsetX = cfg.ScreenWidth * (1.0f / 12.0f);
				int offsetY = cfg.ScreenHeight * 0.0625;
				int x = offsetX * 1.5f;
				int y = cfg.ScreenHeight * 0.75f - offsetY;
				int textOffsetX = cfg.chrNameOffsetX * offsetX;
				if (cfg.chrNameBackGround.valid())
				{
					idList.push_back(world.getEntityManager()->getId());
					float scale = static_cast<float>(offsetY) / static_cast<float>(cfg.chrNameBackGround.get().height);
					world.createUnit(idList.back(), ui::ImageBoxExCom({ Vector2({ static_cast<float>(x), static_cast<float>(y) }), scale, cfg.chrNameBackGround, cfg.textBoxLayer }));
				}

				idList.push_back(world.getEntityManager()->getId());
				world.createUnit(idList.back(), ui::TextBoxExCom{ cfg.fontData, nameBuf, {float(x + textOffsetX), float(y)}, WHITE, cfg.textBoxLayer, float(cfg.textSize), 0.1f * cfg.textSize });
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else
			{
				++nextScene;
			}
		}

		if (sceneType == "TextScene")
		{
			idList.push_back(world.getEntityManager()->getId());
			auto it = viewer.find(targetList.back());
			world.createUnit(idList.back(), visualnovel::TextSceneCom{ exIdList.back(), viewer[targetList.back()]});

		}
	}

	void ApplyScriptLoader(ecs::World2D& world, ScriptLoader& scriptLoader, VisualNovelConfig& cfg)
	{
		ui::ApplyButtonEx(world);
		ui::ApplyImageBoxEx(world);
		ui::ApplyTextBoxEx(world);
		world.addPool<StandardTextBox>();
		world.addSystem(vn::StandardTextBoxSystem(world.getDoubleBuffer<StandardTextBox>(), world.getUiLayer(), cfg.textBoxLayer, cfg));
		world.addPool<visualnovel::TextSceneCom>();
		world.addSystem(TextSceneSystem(world, cfg, scriptLoader));
	}
}
