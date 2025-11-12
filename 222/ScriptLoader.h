#pragma once

#include <fstream>
#include <sstream>
#include <future>

#include "ECS.h"
#include "World.h"
#include "VisualNovel.h"

namespace visualnovel
{
	class MusicManager : public ecs::SystemBase
	{
	private:
		VisualNovelConfig cfg;

		rlRAII::MusicRAII bgm;
		rlRAII::SoundRAII voice;

	public:
		void update() override
		{
			if (bgm.valid())
			{
				UpdateMusicStream(bgm.get());
			}
			
			
		}
	};

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
			while (!isdigit(*nextScene) && *nextScene != '.' && *nextScene != '+' && *nextScene != '-' && !nextScene.eof()) ++nextScene;
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

		ecs::MessageTypeId pressMsgId = world->getMessageManager()->getMessageTypeManager().getId<ui::ButtonPressMsg>();

	public:
		TextSceneSystem(ecs::World2D& world, VisualNovelConfig& cfg, ScriptLoader& scLoader) :
			world(&world), cfg(&cfg), coms(world.getDoubleBuffer<TextSceneCom>()), buttonComs(world.getDoubleBuffer<ui::ButtonExCom>()),
			clicked(false), textBoxComs(world.getDoubleBuffer<StandardTextBox>()), scLoader(&scLoader) {}

		void update() override
		{
			bool processed = false;
			coms->active()->forEach([this, &processed](ecs::entity id, TextSceneCom& comActive)
			{
				if (clicked)
				{
					buttonComs->active()->forEach([this, &processed](ecs::entity id, ui::ButtonExCom& comActive)
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

	struct SelectSceneCom
	{
		std::vector<ecs::entity> buttonIdList;
		std::vector<rlRAII::FileRAII::Iterator> nextSceneList;
	};

	class SelectSceneSystem : public ecs::SystemBase
	{
	private:
		ecs::World2D* world;
		ecs::DoubleComs<SelectSceneCom>* coms;
		ScriptLoader* scLoader;
		VisualNovelConfig* cfg;

		ecs::MessageTypeId pressMsgId = world->getMessageManager()->getMessageTypeManager().getId<ui::ButtonPressMsg>();

	public:
		SelectSceneSystem(ecs::World2D& world, VisualNovelConfig& cfg, ScriptLoader& scLoader) :
			world(&world), cfg(&cfg), coms(world.getDoubleBuffer<SelectSceneCom>()), scLoader(&scLoader) {}

		void update() override
		{
			coms->active()->forEach([this](ecs::entity id, SelectSceneCom& comActive)
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
				nextScene += 4;
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
			else if (!memcmp(nextScene.get(), "Button", 6) && (nextScene[6] == '(' || isspace(nextScene[6])))
			{
				nextScene += 7;
				float relativeX, relativeY, ratio, width;
				std::vector<std::string> languages;
				std::string buf;
				std::string alignment;
				Color textColor;
				rlRAII::Texture2DRAII normalImg;
				rlRAII::Texture2DRAII hoverImg;
				rlRAII::Texture2DRAII pressImg;
				ReadNextNumber(buf, nextScene);
				relativeX = std::stof(buf);
				ReadNextNumber(buf, nextScene);
				relativeY = std::stof(buf);
				ReadNextNumber(buf, nextScene);
				ratio = std::stof(buf);
				ReadNextNumber(buf, nextScene);
				width = std::stof(buf);

				ReadNextString(buf, nextScene);
				languages.push_back(buf);
				ReadNextString(buf, nextScene);
				languages.push_back(buf);
				ReadNextString(buf, nextScene);
				languages.push_back(buf);
				ReadNextString(buf, nextScene);
				languages.push_back(buf);

				ReadNextString(alignment, nextScene);

				ReadNextNumber(buf, nextScene);
				textColor.r = std::stoi(buf);
				ReadNextNumber(buf, nextScene);
				textColor.g = std::stoi(buf);
				ReadNextNumber(buf, nextScene);
				textColor.b = std::stoi(buf);
				ReadNextNumber(buf, nextScene);
				textColor.a = std::stoi(buf);

				ReadNextString(buf, nextScene);
				normalImg = rlRAII::Texture2DRAII(LoadTexture(buf.c_str()));
				ReadNextString(buf, nextScene);
				hoverImg = rlRAII::Texture2DRAII(LoadTexture(buf.c_str()));
				ReadNextString(buf, nextScene);
				pressImg = rlRAII::Texture2DRAII(LoadTexture(buf.c_str()));

				float offsetX = cfg.ScreenWidth * relativeX, offsetY = cfg.ScreenHeight * relativeY;
				float height = width / ratio;
				if (alignment == "Center")
				{
					offsetX -= (width * cfg.ScreenWidth) / 2.0f;
					offsetY -= (height * cfg.ScreenHeight) / 2.0f;
				}
				
				
				if (sceneType == "SelectScene")
				{
					exIdList.push_back(world.getEntityManager()->getId());
					SetTextureFilter(normalImg.get(), RL_TEXTURE_FILTER_BILINEAR);
					SetTextureFilter(hoverImg.get(), RL_TEXTURE_FILTER_BILINEAR);
					SetTextureFilter(pressImg.get(), RL_TEXTURE_FILTER_BILINEAR);
					world.createUnit(exIdList.back(), ui::ButtonExCom
					{
						cfg.fontData,
						normalImg,
						hoverImg,
						pressImg,
						languages[cfg.uiLanguage],
						textColor,
						cfg.textSize,
						int(cfg.textSize * 0.1),
						Vector2{ offsetX, offsetY },
						Vector2{ width * cfg.ScreenWidth, height * cfg.ScreenWidth },
						cfg.ButtonLayer,
						(width * cfg.ScreenWidth) / normalImg.get().width
					});
					world.getMessageManager()->subscribe(exIdList.back());
				}
			}
			else if (!memcmp(nextScene.get(), "//", 2))
			{
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
		else if (sceneType == "SelectScene")
		{
			idList.push_back(world.getEntityManager()->getId());
			std::vector<rlRAII::FileRAII::Iterator> targetItList;
			for (auto target : targetList)
			{
				auto it = viewer.find(target);
				targetItList.push_back(it->second);
			}
			world.createUnit(idList.back(), visualnovel::SelectSceneCom{ exIdList, targetItList });
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
		world.addPool<visualnovel::SelectSceneCom>();
		world.addSystem(SelectSceneSystem(world, cfg, scriptLoader));
	}
}
