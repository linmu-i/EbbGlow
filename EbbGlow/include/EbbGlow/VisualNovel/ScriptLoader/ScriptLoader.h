#pragma once

#include <string>

#include <EbbGlow/Core/World.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptData.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>
#include <EbbGlow/VisualNovel/ScriptLoader/MusicManager.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoaderTypes.h>
#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/UI/UI.h>

namespace ebbglow::visualnovel
{
	class ScriptLoader
	{
	private:
		//处理函数
		std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>> globalFunctions;
		std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>> sceneFunctions;
		std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>> sceneCreator;

		Command Tokenizer(rsc::SharedFile::Iterator& it) noexcept;
		void Invoker(const Command& cmd, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept;
		void ExecuteFunction(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept;
		void ExecuteMacro(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept;
		SceneInfo ReadSceneInfo(rsc::SharedFile::Iterator& it) noexcept;

	public:
		core::World2D& world;
		MusicManager& musicMgr;

		//变量存储
		std::vector<double> numberStorage;
		std::vector<std::string> textStorage;
		std::unordered_map<std::string, VariableView> numberView;
		std::unordered_map<std::string, VariableView> textView;

		//宏函数
		std::unordered_map<std::string, MacroView> macroView;

		//定位缓存
		std::unordered_map<std::string, int64_t> sceneView;

		//文本回看
		std::vector<LogView> logView;

		//初始化数据
		ScriptData scriptData;
		std::string beginScene;

		//运行时数据
		std::vector<core::entity> idList;
		std::vector<core::entity> exIdList;
		std::vector<std::string> sceneArgs;
		std::string sceneName;
		std::string sceneType;
		rsc::SharedFile::Iterator scIt;//解析时使用的迭代器，供追加文本时更新迭代器使用
		LogView logTmp;

		//设置
		VisualNovelConfig& cfg;

		ScriptLoader(core::World2D& world, VisualNovelConfig& cfg, MusicManager* musicMgr) : world(world), cfg(cfg), musicMgr(*musicMgr) {}
		std::future<void> init(const std::string& filePath) noexcept;
		void loadScene(rsc::SharedFile::Iterator& it) noexcept;
		void start() noexcept;
		void registerSceneType(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept;
		void registerSceneFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept;
		void registerGlobalFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept;
		void addLog(const LogView& logView) noexcept;
		void addLog(LogView&& logView) noexcept;
	};
	 
	void SkipSpace(rsc::SharedFile::Iterator& ptr);
	double ParsePrimary(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseTerm(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseExpression(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	std::string GetStateTag(std::string_view token);
	std::string GetNextString(rsc::SharedFile::Iterator& it, ScriptLoader& scLoader);
	std::string GetString(std::string_view token, ScriptLoader& scLoader);
	double GetNumber(std::string_view token, unsigned char stop, ScriptLoader& scLoader);
	std::string* GetTextVariable(const std::string& name, ScriptLoader& scLoader) noexcept;
	double* GetNumberVariable(const std::string& name, ScriptLoader& scLoader) noexcept;
}






/*
namespace ebbglow::visualnovel
{
	struct SceneView
	{
		enum class SceneType : uint8_t
		{
			TYPE_TEXTSCENE = 0,
			TYPE_SELECTSCENE = 1
		};
		std::string sceneName;
		SceneType sceneType;
		std::string text;
	};

	enum class VarType : uint8_t
	{
		Str,
		Num
	};

	enum class BackgroundDrawType : uint8_t
	{
		Contain,
		Cover
	};

	struct VariableView
	{
		VarType type;
		uint32_t offset;
		uint16_t length;
	};

	struct MacroView
	{
		std::vector<std::string> argsList;
		std::string body;
	};

	class ScriptLoader
	{
	private:
		core::World2D& world;
		ScriptData script;
		visualnovel::VisualNovelConfig& cfg;

		std::unordered_map<std::string, rsc::SharedFile::Iterator> viewer;
		std::unordered_map<std::string, SceneView> logs;
		std::vector<std::string> targetScene;
		std::vector<core::entity> idList;
		std::vector<core::entity> exIdList;

		std::vector<std::string> StringStorage;
		std::vector<double> NumberStorage;

		std::string beginScene;

		std::unordered_map<std::string, std::function<void()>()> globalSectionFunctions;
		std::unordered_map<std::string, std::function<void()>()> sceneSectionFunctions;

		
		std::unordered_map<std::string, VariableView> variables;

		
		std::unordered_map<std::string, MacroView> macros;

		friend void ApplyScriptLoader(core::World2D&, ScriptLoader&, VisualNovelConfig&);
		friend std::string GetNextString(rsc::SharedFile::Iterator& nextScene, ScriptLoader& scLoader);
		MusicManager* mscMgr = nullptr;

		friend double ParsePrimary(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
		void CreateTextScene(const std::vector<std::string>& languages, core::entity textBoxId, rsc::SharedTexture2D backGround, bool read, BackgroundDrawType drawType)
		{
			world.createUnit(textBoxId, vn::MainTextBoxCom
			(
				languages[cfg.mainLanguage],
				languages[cfg.secondaryLanguage],
				cfg.textSize, cfg.fontData, cfg.textSpeed, Vector2({ cfg.ScreenWidth * 0.1666667f, cfg.ScreenHeight * 0.75f }) + cfg.drawOffset, cfg.ScreenWidth * 0.6666667f,
				cfg.showReadText && read ? cfg.readTextColor : WHITE
			));

			float scaleX = static_cast<float>(cfg.ScreenWidth) / static_cast<float>(backGround.get().width);
			float scaleY = static_cast<float>(cfg.ScreenHeight) / static_cast<float>(backGround.get().height);

			float bgScale;
			Vector2 bgPosition;
			switch (drawType)
			{
			default:
			case BackgroundDrawType::Contain:
				bgScale = std::min(scaleX, scaleY);
				if (scaleX > scaleY)
				{
					bgPosition = { (cfg.ScreenWidth - backGround.get().width * bgScale) / 2.0f, 0.0f };
				}
				else
				{
					bgPosition = { 0.0f, (cfg.ScreenHeight - backGround.get().height * bgScale) / 2.0f };
				}
				break;

			case BackgroundDrawType::Cover:
				bgScale = std::max(scaleX, scaleY);
				if (scaleX < scaleY)
				{
					bgPosition = { (cfg.ScreenWidth - backGround.get().width * bgScale) / 2.0f, 0.0f };
				}
				else
				{
					bgPosition = { 0.0f, (cfg.ScreenHeight - backGround.get().height * bgScale) / 2.0f };
				}
			}


			auto idMgr = world.getEntityManager();
			if (backGround.valid())
			{
				idList.push_back(idMgr->getId());
				world.createUnit(idList.back(), ui::ImageBoxExCom({ bgPosition + cfg.drawOffset, bgScale, backGround, cfg.LayerDefine.backGroundLayer }));
			}
			if (cfg.textBoxBackGround.valid())
			{
				float scale = std::max(float(cfg.ScreenWidth) / cfg.textBoxBackGround.get().width, float(cfg.ScreenHeight * (0.25f + 0.03125f)) / cfg.textBoxBackGround.get().height);
				idList.push_back(idMgr->getId());
				world.createUnit(idList.back(), ui::ImageBoxExCom({ Vector2{ (cfg.ScreenWidth - cfg.textBoxBackGround.get().width * scale) / 2, cfg.ScreenHeight * (0.75f - 0.03125f)} + cfg.drawOffset, scale, cfg.textBoxBackGround, cfg.LayerDefine.textBoxBackGroundLayer }));
			}
		}


		void LoadSceneInfo(rsc::SharedFile::Iterator& nextScene, std::string& sceneName, std::string& sceneType, std::vector<std::string>& argsList)
		{
			std::string targetTmp;
			while (memcmp(nextScene.get(), "Scene", 5)) ++nextScene;
			nextScene += 5;
			while (isspace(*nextScene) && !nextScene.eof()) ++nextScene;
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
			while (*nextScene != ')' && !nextScene.eof())
			{
				if (!isspace(*nextScene) && *nextScene != ',')
				{
					targetTmp += *nextScene;
				}
				if (*nextScene == ',')
				{
					argsList.push_back(targetTmp);
					targetTmp.clear();
				}
				++nextScene;
			}
			argsList.push_back(targetTmp);
			while (*nextScene != '\n' && !nextScene.eof())	++nextScene;
			++nextScene;
		}
		void LoadFunction(rsc::SharedFile::Iterator& nextScene, std::string& sceneName, std::string& sceneType, std::string& textTmp, std::vector<std::string> argsList)
		{
			if (!memcmp(nextScene.get(), "TextScene", 9) && (nextScene[9] == '(' || isspace(nextScene[9])))
			{
				std::string textBuf;
				std::vector<std::string> texts;
				std::string bg;
				nextScene += 10;
				textBuf = GetString(nextScene);
				texts.push_back(textBuf);
				textBuf = GetString(nextScene);
				texts.push_back(textBuf);
				textBuf = GetString(nextScene);
				texts.push_back(textBuf);
				textBuf = GetString(nextScene);
				texts.push_back(textBuf);
				textBuf = GetString(nextScene);
				bg = textBuf;
				ReadNextStateTag(textBuf, nextScene);
				auto readIt = cfg.readTextSet.find(sceneName);
				BackgroundDrawType bgType;
				textTmp = texts[cfg.mainLanguage];
				if (textBuf == "Cover")
				{
					bgType = BackgroundDrawType::Cover;
				}
				else
				{
					bgType = BackgroundDrawType::Contain;
				}
				if (sceneType == "TextScene")
				{
					exIdList.push_back(world.getEntityManager()->getId());
					char* ansi = LoadANSI(bg.c_str(), bg.size());
					CreateTextScene(texts, exIdList.back(), rsc::SharedTexture2D(ansi), readIt == cfg.readTextSet.end(), bgType);
					UnloadANSI(ansi);
				}
				else
				{
					idList.push_back(world.getEntityManager()->getId());
					char* ansi = LoadANSI(bg.c_str(), bg.size());
					CreateTextScene(texts, idList.back(), rsc::SharedTexture2D(ansi), readIt == cfg.readTextSet.end(), bgType);
					UnloadANSI(ansi);
				}
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "Chr", 3) && (nextScene[3] == '(' || isspace(nextScene[3])))
			{
				nextScene += 4;
				std::string nameBuf;
				nameBuf = GetString(nextScene);

				int offsetX = cfg.ScreenWidth * (1.0f / 12.0f);
				int offsetY = cfg.ScreenHeight * 0.0625;
				int x = offsetX * 1.5f;
				int y = cfg.ScreenHeight * 0.75f - offsetY;
				int textOffsetX = cfg.chrNameOffsetX * offsetX;
				if (cfg.chrNameBackGround.valid())
				{
					idList.push_back(world.getEntityManager()->getId());
					float scale = static_cast<float>(offsetY) / static_cast<float>(cfg.chrNameBackGround.get().height);
					world.createUnit(idList.back(), ui::ImageBoxExCom({ Vector2{ static_cast<float>(x), static_cast<float>(y) } + cfg.drawOffset, scale, cfg.chrNameBackGround, cfg.LayerDefine.textBoxLayer }));
				}

				idList.push_back(world.getEntityManager()->getId());
				world.createUnit(idList.back(), ui::TextBoxExCom{ cfg.fontData, nameBuf, Vector2{float(x + textOffsetX), float(y)} + cfg.drawOffset, WHITE, cfg.LayerDefine.textBoxLayer, float(cfg.textSize), 0.1f * cfg.textSize });
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "Button", 6) && (nextScene[6] == '(' || isspace(nextScene[6])))
			{
				nextScene += 6;
				while (*nextScene != '(') ++nextScene;
				++nextScene;
				float relativeX, relativeY, ratio, width;
				std::vector<std::string> languages;
				std::string buf;
				std::string alignment;
				Color textColor;
				rsc::SharedTexture2D normalImg;
				rsc::SharedTexture2D hoverImg;
				rsc::SharedTexture2D pressImg;
				relativeX = GetNumber(nextScene, ',');
				relativeY = GetNumber(nextScene, ',');
				ratio = GetNumber(nextScene, ',');
				width = GetNumber(nextScene, ',');

				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				languages.push_back(buf);
				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				languages.push_back(buf);
				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				languages.push_back(buf);
				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				languages.push_back(buf);

				ReadNextStateTag(alignment, nextScene);

				textColor.r = GetNumber(nextScene, ',');
				textColor.g = GetNumber(nextScene, ',');
				textColor.b = GetNumber(nextScene, ',');
				textColor.a = GetNumber(nextScene, ',');

				buf = GetString(nextScene);
				char* ansi = LoadANSI(buf.c_str(), buf.size());
				normalImg = rsc::SharedTexture2D(ansi);
				UnloadANSI(ansi);

				buf = GetString(nextScene);
				ansi = LoadANSI(buf.c_str(), buf.size());
				hoverImg = rsc::SharedTexture2D(ansi);
				UnloadANSI(ansi);

				buf = GetString(nextScene);
				ansi = LoadANSI(buf.c_str(), buf.size());
				pressImg = rsc::SharedTexture2D(ansi);
				UnloadANSI(ansi);

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
					SetTextureFilter(normalImg.get(), TEXTURE_FILTER_BILINEAR);
					SetTextureFilter(hoverImg.get(), TEXTURE_FILTER_BILINEAR);
					SetTextureFilter(pressImg.get(), TEXTURE_FILTER_BILINEAR);
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
							Vector2{ offsetX, offsetY } + cfg.drawOffset,
							Vector2{ width * cfg.ScreenWidth, height * cfg.ScreenWidth },
							cfg.LayerDefine.ButtonLayer,
							(width * cfg.ScreenWidth) / normalImg.get().width
						});
					world.getMessageManager()->subscribe(exIdList.back());
				}
				else
				{
					idList.push_back(world.getEntityManager()->getId());
					SetTextureFilter(normalImg.get(), TEXTURE_FILTER_BILINEAR);
					SetTextureFilter(hoverImg.get(), TEXTURE_FILTER_BILINEAR);
					SetTextureFilter(pressImg.get(), TEXTURE_FILTER_BILINEAR);
					world.createUnit(idList.back(), ui::ButtonExCom
						{
							cfg.fontData,
							normalImg,
							hoverImg,
							pressImg,
							languages[cfg.uiLanguage],
							textColor,
							cfg.textSize,
							int(cfg.textSize * 0.1),
							Vector2{ offsetX, offsetY } + cfg.drawOffset,
							Vector2{ width * cfg.ScreenWidth, height * cfg.ScreenWidth },
							cfg.LayerDefine.ButtonLayer,
							(width * cfg.ScreenWidth) / normalImg.get().width
						});
					world.getMessageManager()->subscribe(idList.back());
				}

				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "Image", 5) && (nextScene[5] == '(' || isspace(nextScene[5])))
			{
				nextScene += 5;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				float relativeX, relativeY, ratio, width, scale, x, y;
				rsc::SharedTexture2D img;
				std::string buf;
				relativeX = GetNumber(nextScene, ',');
				relativeY = GetNumber(nextScene, ',');
				ratio = GetNumber(nextScene, ',');
				width = GetNumber(nextScene, ',');
				buf = GetString(nextScene);
				char* ansi = LoadANSI(buf.c_str(), buf.size());
				img = rsc::SharedTexture2D(LoadTexture(ansi));
				UnloadANSI(ansi);
				ReadNextStateTag(buf, nextScene);
				if (buf == "Cover")
				{
					scale = std::max(width * cfg.ScreenWidth / img.get().width, width / ratio * cfg.ScreenWidth / img.get().height);
				}
				else
				{
					scale = std::min(width * cfg.ScreenWidth / img.get().width, width / ratio * cfg.ScreenWidth / img.get().height);
				}
				ReadNextStateTag(buf, nextScene);
				if (buf == "Center")
				{
					x = (relativeX * cfg.ScreenWidth - img.get().width * scale / 2);
					y = (relativeY * cfg.ScreenHeight - img.get().height * scale / 2);
				}
				else
				{
					x = relativeX * cfg.ScreenWidth;
					y = relativeY * cfg.ScreenHeight;
				}
				int layerDepth = (int)int(GetNumber(nextScene, ')'));
				idList.push_back(world.getEntityManager()->getId());
				world.createUnit(idList.back(), ui::ImageBoxExCom{ Vector2{x, y} + cfg.drawOffset, scale, img, layerDepth });

				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "SetStr", 6) && (nextScene[6] == '(' || isspace(nextScene[6])))
			{
				nextScene += 6;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				std::string varName;
				while (*nextScene != '$' && !nextScene.eof()) ++nextScene;
				++nextScene;
				while (!isspace(*nextScene) && !nextScene.eof() && *nextScene != ',' && *nextScene != '[')
				{
					varName += *nextScene;
					++nextScene;
				}
				int idx = 0;
				if (*nextScene == '[')
				{
					++nextScene;
					idx = (int)round(GetNumber(nextScene, ']'));
					--nextScene;//GetNumber会跳过','，这里为了匹配通用跳跃逻辑
				}
				while (!nextScene.eof() && *nextScene != ',') ++nextScene;
				++nextScene;
				std::string value = GetString(nextScene);
				if (varName != "TARGET_LIST" && varName != "TEXTBOX_BACKGROUND" && varName != "CHR_NAME_BACKGROUND")
				{
					auto varRef = GetVariable(varName);
					if (varRef.type == VarType::Str && varRef.length > 0)
					{
						StringStorage[varRef.offset + idx] = value;
					}
				}
				else
				{
					if (varName == "TARGET_LIST" && idx < argsList.size())
					{
						argsList[idx] = value;
					}

				}

				while (!nextScene.eof() && *nextScene != '\n') ++nextScene;
				++nextScene;

			}
			else if (!memcmp(nextScene.get(), "SetNum", 6) && (nextScene[6] == '(' || isspace(nextScene[6])))
			{
				nextScene += 6;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				std::string varName;
				while (*nextScene != '$' && !nextScene.eof()) ++nextScene;
				++nextScene;
				while (!isspace(*nextScene) && !nextScene.eof() && *nextScene != ',' && *nextScene != '[')
				{
					varName += *nextScene;
					++nextScene;
				}
				int idx = 0;
				if (*nextScene == '[')
				{
					++nextScene;
					idx = (int)round(GetNumber(nextScene, ']'));
					while (*nextScene != ']' && !nextScene.eof()) ++nextScene;
					++nextScene;
				}
				while (!nextScene.eof() && *nextScene != ',') ++nextScene;
				++nextScene;
				double value = GetNumber(nextScene, ')');
				auto varRef = GetVariable(varName);
				if (varRef.type == VarType::Num && varRef.length > 0)
				{
					NumberStorage[varRef.offset + idx] = value;
				}
				while (!nextScene.eof() && *nextScene != '\n') ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "if", 2) && (nextScene[2] == '(' || isspace(nextScene[2])))
			{
				nextScene += 2;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				double var0 = f2(nextScene, ')');
				bool cond = false;
				SkipSpaceUntil(nextScene, ')');
				if (*nextScene == '<')
				{
					if (nextScene[1] == '=')
					{
						nextScene += 2;
						cond = (var0 <= f2(nextScene, ')'));
					}
					else
					{
						nextScene += 1;
						cond = (var0 < f2(nextScene, ')'));
					}
				}
				else if (*nextScene == '>')
				{
					if (nextScene[1] == '=')
					{
						nextScene += 2;
						cond = (var0 >= f2(nextScene, ')'));
					}
					else
					{
						nextScene += 1;
						cond = (var0 > f2(nextScene, ')'));
					}
				}
				else if (*nextScene == '=')
				{
					if (nextScene[1] == '=')
					{
						nextScene += 2;
						cond = (var0 == f2(nextScene, ')'));
					}
				}
				else if (*nextScene == '!')
				{
					if (nextScene[1] == '=')
					{
						nextScene += 2;
						cond = (var0 != f2(nextScene, ')'));
					}
				}
				else
				{
					cond = false;
				}
				if (cond)
				{
					while (*nextScene != '{' && !nextScene.eof()) ++nextScene;
					++nextScene;
				}
				else
				{
					while (*nextScene != '{' && !nextScene.eof()) ++nextScene;
					++nextScene;
					int braceCount = 1;
					while (!nextScene.eof() && braceCount > 0)
					{
						if (*nextScene == '{') ++braceCount;
						else if (*nextScene == '}') --braceCount;
						else if (*nextScene == '(')
						{
							int braceCount2 = 1;
							++nextScene;
							while (!nextScene.eof() && braceCount2 > 0)
							{
								if (*nextScene == '"')
								{
									++nextScene;
									while (!nextScene.eof() && *nextScene != '"')
									{
										if (*nextScene == '\\' && nextScene.position() < nextScene.size() - 1)
										{
											nextScene += 2;
										}
										else
										{
											++nextScene;
										}
									}
								}
								if (*nextScene == '(') ++braceCount2;
								else if (*nextScene == ')') --braceCount2;
								++nextScene;
							}
						}
						else if (*nextScene == '/')
						{
							if (nextScene[1] == '/')
							{
								while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
							}
						}
						++nextScene;
					}
				}
			}
			else if (!memcmp(nextScene.get(), "TextBox", 7) && (nextScene[7] == '(' || isspace(nextScene[7])))
			{
				nextScene += 7;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				double relativeX, relativeY, textRelativeSize, r, g, b, a, layerDepth;
				relativeX = GetNumber(nextScene, ')');
				relativeY = GetNumber(nextScene, ')');
				textRelativeSize = GetNumber(nextScene, ')');
				textRelativeSize *= std::min(cfg.ScreenWidth / 1920.0f, cfg.ScreenHeight / 1080.0f);
				int offsetX = cfg.ScreenWidth * relativeX;
				int offsetY = cfg.ScreenHeight * relativeY;
				std::string text = GetString(nextScene);
				r = GetNumber(nextScene, ')');
				g = GetNumber(nextScene, ')');
				b = GetNumber(nextScene, ')');
				a = GetNumber(nextScene, ')');
				std::string fontPath = GetString(nextScene);
				rsc::SharedFile fontFile;
				if (fontPath.empty())
				{
					fontFile = cfg.fontData;
				}
				else
				{
					fontFile = rsc::SharedFile(fontPath.c_str());
					if (!fontFile.valid())
					{
						fontFile = cfg.fontData;
					}
				}
				layerDepth = GetNumber(nextScene, ')');
				std::string state;
				ReadNextStateTag(state, nextScene);
				rsc::SharedFont font = DynamicLoadFontFromMemory(text.c_str(), cfg.fontData.fileName(), cfg.fontData.get(), cfg.fontData.size(), static_cast<int>(textRelativeSize));
				if (state == "Center")
				{
					Vector2 cover = MeasureTextEx(font.get(), text.c_str(), float(textRelativeSize), float(textRelativeSize) * 0.1f);
					idList.push_back(world.getEntityManager()->getId());
					world.createUnit(idList.back(), ui::TextBoxExCom{ fontFile, text, Vector2{ float(offsetX) - cover.x / 2.0f, float(offsetY) - cover.y / 2.0f } + cfg.drawOffset, Color{uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(a)}, (int)round(layerDepth), float(textRelativeSize), float(textRelativeSize) * 0.1f });
				}
				else
				{
					idList.push_back(world.getEntityManager()->getId());
					world.createUnit(idList.back(), ui::TextBoxExCom{ fontFile, text, Vector2{ float(offsetX), float(offsetY) } + cfg.drawOffset, Color{uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(a)}, (int)round(layerDepth), float(textRelativeSize), float(textRelativeSize) * 0.1f });
				}
			}
			else if (!memcmp(nextScene.get(), "KeyFrameAnimation", 17) && (nextScene[17] == '(' || isspace(nextScene[17])))
			{
				nextScene += 17;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				std::string imgPath = GetString(nextScene);
				char* imgPathAnsi = LoadANSI(imgPath.c_str(), imgPath.size());
				rsc::SharedTexture2D img(imgPathAnsi);
				UnloadANSI(imgPathAnsi);
				float scale = cfg.ScreenWidth / 1920.0f;
				int layerDepth = GetNumber(nextScene, ')');
				std::string isLoop;
				ReadNextStateTag(isLoop, nextScene);
				idList.push_back(world.getEntityManager()->getId());
				world.createUnit(idList.back(), ui::KeyFramesAnimationCom{ img, world.getUiLayer(), scale, layerDepth, isLoop == "Loop" ? true : false });
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "AddKeyFrame", 11) && (nextScene[11] == '(' || isspace(nextScene[11])))
			{
				nextScene += 11;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;

				auto* animationActive = world.getDoubleBuffer<ui::KeyFramesAnimationCom>()->active()->get(idList.back());
				auto* animationInactive = world.getDoubleBuffer<ui::KeyFramesAnimationCom>()->inactive()->get(idList.back());

				if (animationActive && animationInactive)
				{
					ui::KeyFrame keyFrame;
					float relativeX = GetNumber(nextScene, ',');
					float relativeY = GetNumber(nextScene, ',');
					float originX = GetNumber(nextScene, ',');
					float originY = GetNumber(nextScene, ',');
					float scale = GetNumber(nextScene, ',');
					float rotation = GetNumber(nextScene, ',');
					float duration = GetNumber(nextScene, ',');
					uint8_t alpha = (uint8_t)round(GetNumber(nextScene, ','));
					std::string state;
					ReadNextStateTag(state, nextScene);

					keyFrame.origin = Vector2{ originX * animationActive->texture.get().width, originY * animationActive->texture.get().height };
					keyFrame.duration = duration;
					keyFrame.rotation = rotation;
					keyFrame.scale = scale;
					keyFrame.alpha = alpha;

					if (state == "Center")
					{
						keyFrame.position = Vector2{ relativeX * cfg.ScreenWidth - animationActive->texture.get().width * animationActive->scale / 2.0f * scale, relativeY * cfg.ScreenHeight - animationActive->texture.get().height * animationActive->scale / 2.0f * scale };
					}
					else
					{
						keyFrame.position = Vector2{ relativeX * cfg.ScreenWidth, relativeY * cfg.ScreenHeight };
					}
					animationInactive->keyFrames.push_back(keyFrame);
				}
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "SetBGM", 6) && (nextScene[6] == '(' || isspace(nextScene[6])))
			{
				nextScene += 6;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				auto path = GetString(nextScene);
				char* ansi = LoadANSI(path.c_str(), path.size());
				rsc::SharedMusic bgm(ansi);
				UnloadANSI(ansi);
				int volumeIdx = (int)round(GetNumber(nextScene, ')'));
				mscMgr->SetBGM(bgm, cfg.volumes[volumeIdx]);
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "SetVoice", 8) && (nextScene[8] == '(' || isspace(nextScene[6])))
			{
				nextScene += 8;
				while (*nextScene != '(' && !nextScene.eof()) ++nextScene;
				++nextScene;
				auto path = GetString(nextScene);
				char* ansi = LoadANSI(path.c_str(), path.size());
				rsc::SharedSound voice(ansi);
				UnloadANSI(ansi);
				int volumeIdx = (int)round(GetNumber(nextScene, ')'));
				mscMgr->SetVoice(voice, cfg.volumes[volumeIdx]);
				while (*nextScene != '\n' && !nextScene.eof()) ++nextScene;
				++nextScene;
			}
			else if (!memcmp(nextScene.get(), "&", 1))
			{
				++nextScene;
				std::string macroName;
				while (!isspace(*nextScene) && *nextScene != '(' && !nextScene.eof())
				{
					macroName += *nextScene;
					++nextScene;
				}
				while (isspace(*nextScene) && !nextScene.eof()) ++nextScene;
				++nextScene;
				std::vector<std::string> macroArgs;
				while (*nextScene != ')' && !nextScene.eof())
				{
					std::string argBuf;
					while (isspace(*nextScene) && !nextScene.eof()) ++nextScene;
					while (*nextScene != ',' && *nextScene != ')' && !isspace(*nextScene) && !nextScene.eof())
					{
						argBuf += *nextScene;
						++nextScene;
					}
					macroArgs.push_back(argBuf);
					while (isspace(*nextScene) && !nextScene.eof()) ++nextScene;
					if (*nextScene == ',') ++nextScene;
				}
				++nextScene;
				auto macroIt = macros.find(macroName);
				if (macroIt != macros.end())
				{
					std::vector<unsigned char> macroScript;
					auto bodyIt = macroIt->second.body.begin();
					while (bodyIt != macroIt->second.body.end())
					{
						std::string argName;
						if (*bodyIt != '$')
						{
							macroScript.push_back(*bodyIt);
							++bodyIt;
							continue;
						}
						else
						{
							++bodyIt;
							while (!isspace(*bodyIt) && *bodyIt != ',' && *bodyIt != ')')
							{
								argName += *bodyIt;
								++bodyIt;
							}
							size_t index = 0;
							for (auto argN : macroIt->second.argsList)
							{
								if (argN == argName) break;
								++index;
							}
							if (index < macroArgs.size())
							{
								for (auto c : macroArgs[index])
								{
									macroScript.push_back(c);
								}
							}
						}
					}
					rsc::SharedFile::Iterator macroIt2(macroScript.size(), macroScript.data());
					while (!macroIt2.eof())
					{
						LoadFunction(macroIt2, sceneName, sceneType, textTmp, argsList);
					}
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
		void LoadGlobal(rsc::SharedFile::Iterator& scriptPtr, ScriptData& scriptDataTmp)
		{
			while (*scriptPtr != '\n' && !scriptPtr.eof()) ++scriptPtr;
			++scriptPtr;
			while (memcmp(scriptPtr.get(), "Scene", 5) && !scriptPtr.eof())
			{
				if (!memcmp(scriptPtr.get(), "String", 6))
				{
					scriptPtr += 6;
					int length = 1;
					if (*scriptPtr == '[')
					{
						length = std::max(1, (int)round(GetNumber(scriptPtr, ']')));
						while (!scriptPtr.eof() && *scriptPtr != ']') ++scriptPtr;
						++scriptPtr;
					}
					SkipSpaceUntil(scriptPtr, '\n');
					std::string varName;
					while (!scriptPtr.eof() && !isspace(*scriptPtr))
					{
						varName += *scriptPtr;
						++scriptPtr;
					}
					StringStorage.resize(StringStorage.size() + length);
					variables[varName] = VariableView{ VarType::Str, static_cast<uint32_t>(StringStorage.size() - length), static_cast<uint16_t>(length) };
					while (*scriptPtr != '\n' && !scriptPtr.eof()) ++scriptPtr;
					++scriptPtr;
				}
				else if (!memcmp(scriptPtr.get(), "Number", 6))
				{
					scriptPtr += 6;
					int length = 1;
					if (*scriptPtr == '[')
					{
						length = std::max(1, (int)round(GetNumber(scriptPtr, ']')));
						while (!scriptPtr.eof() && *scriptPtr != ']') ++scriptPtr;
						++scriptPtr;
					}
					SkipSpaceUntil(scriptPtr, '\n');
					std::string varName;
					while (!scriptPtr.eof() && !isspace(*scriptPtr))
					{
						varName += *scriptPtr;
						++scriptPtr;
					}
					NumberStorage.resize(NumberStorage.size() + length);
					variables[varName] = VariableView{ VarType::Num, static_cast<uint32_t>(NumberStorage.size() - length), static_cast<uint16_t>(length) };
					while (*scriptPtr != '\n' && !scriptPtr.eof()) ++scriptPtr;
					++scriptPtr;
				}
				else if (!memcmp(scriptPtr.get(), "include", 7))
				{
					scriptPtr += 7;
					SkipSpaceUntil(scriptPtr, '(');
					++scriptPtr;
					std::string scPath;
					scPath = GetString(scriptPtr);
					char* ANSIPath = LoadANSI(scPath.c_str(), scPath.size());
					ScriptData tmp(ANSIPath);
					UnloadANSI(ANSIPath);
					scriptDataTmp += std::move(tmp);
					scriptPtr = rsc::SharedFile::Iterator(scriptDataTmp.getSize(), scriptDataTmp.getData(), scriptPtr.position());
					while (*scriptPtr != '\n' && !scriptPtr.eof()) ++scriptPtr;
					++scriptPtr;
				}
				else if (!memcmp(scriptPtr.get(), "Begin", 5))
				{
					scriptPtr += 5;
					SkipSpaceUntil(scriptPtr, '(');
					++scriptPtr;
					beginScene = GetString(scriptPtr);
					while (*scriptPtr != '\n') ++scriptPtr;
					++scriptPtr;
				}
				else if (!memcmp(scriptPtr.get(), "Macro", 5))
				{
					scriptPtr += 5;
					while (isspace(*scriptPtr)) ++scriptPtr;
					std::string macroName;
					while (!isspace(*scriptPtr) && *scriptPtr != '(' && !scriptPtr.eof())
					{
						macroName += *scriptPtr;
						++scriptPtr;
					}
					while (isspace(*scriptPtr)) ++scriptPtr;
					++scriptPtr;
					MacroView macroViewTmp;
					while (*scriptPtr != ')' && !scriptPtr.eof())
					{
						while (isspace(*scriptPtr) && !scriptPtr.eof()) ++scriptPtr;
						std::string argBuf;
						while (*scriptPtr != ',' && *scriptPtr != ')' && !isspace(*scriptPtr) && !scriptPtr.eof())
						{
							argBuf += *scriptPtr;
							++scriptPtr;
						}
						macroViewTmp.argsList.push_back(argBuf);
						while (isspace(*scriptPtr) && !scriptPtr.eof()) ++scriptPtr;
						if (*scriptPtr == ',') ++scriptPtr;
					}
					++scriptPtr;
					while (*scriptPtr != '{' && !scriptPtr.eof()) ++scriptPtr;
					++scriptPtr;
					int braceCount = 1;
					std::string macroScriptBuf;
					macroScriptBuf.reserve(128);
					while (braceCount)
					{
						if (*scriptPtr == '{')
						{
							++braceCount;
						}
						else if (*scriptPtr == '}')
						{
							--braceCount;
						}
						else if (*scriptPtr == '(')
						{
							int braceCount2 = 1;
							macroScriptBuf.push_back(*scriptPtr);
							++scriptPtr;
							while (braceCount2 > 0 && !scriptPtr.eof())
							{
								if (*scriptPtr == '"')
								{
									macroScriptBuf.push_back(*scriptPtr);
									++scriptPtr;
									while (!scriptPtr.eof() && *scriptPtr != '"')
									{
										if (*scriptPtr == '\\' && scriptPtr.position() < scriptPtr.size() - 1)
										{
											macroScriptBuf.push_back(*scriptPtr);
											++scriptPtr;
											macroScriptBuf.push_back(*scriptPtr);
											++scriptPtr;
										}
										else
										{
											macroScriptBuf.push_back(*scriptPtr);
											++scriptPtr;
										}
									}
								}
								if (*scriptPtr == '(') ++braceCount2;
								else if (*scriptPtr == ')') --braceCount2;
								macroScriptBuf.push_back(*scriptPtr);
								++scriptPtr;
							}
							continue;
						}
						else if (*scriptPtr == '/')
						{
							if (scriptPtr[1] == '/')
							{
								macroScriptBuf.push_back(*scriptPtr);
								++scriptPtr;
								macroScriptBuf.push_back(*scriptPtr);
								++scriptPtr;
								while (*scriptPtr != '\n' && !scriptPtr.eof())
								{
									macroScriptBuf.push_back(*scriptPtr);
									++scriptPtr;
								}
								continue;
							}
						}
						if (braceCount > 0)
						{
							macroScriptBuf.push_back(*scriptPtr);
						}
						++scriptPtr;
					}
					macroViewTmp.body = std::move(macroScriptBuf);
					macros.emplace(macroName, std::move(macroViewTmp));
				}
				else
				{
					++scriptPtr;
				}
			}
		}

	public:
		ScriptLoader(core::World2D& world, const std::string& script, vn::VisualNovelConfig& cfg) : world(world), script(script.c_str()), cfg(cfg) {}

		void start()
		{
			auto s = viewer.find(beginScene);
			if (s != viewer.end())
			{
				loadScene(s->second);
			}
		}

		std::future<void> load()
		{
			auto scriptPtr = script.begin();
			if (memcmp(scriptPtr.get(), "Global", 6)) return std::async(std::launch::deferred, []() {});
			std::pair<std::string, int64_t> beginTmp;
			return std::async(std::launch::async, [this]()
				{
					auto iter = script.begin();
					std::vector<std::pair<std::string, int64_t>> mapTmp;
					while (iter)
					{
						if (!memcmp(iter.get(), "Scene", 5) && (iter.position() == 0 || iter[-1] == '\n') && (isspace(iter[5])))
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
							mapTmp.emplace_back(stName, tmpIter.position());
						}
						else if (!memcmp(iter.get(), "Global", 6) && (iter.position() == 0 || iter[-1] == '\n') && (isspace(iter[6])))
						{
							LoadGlobal(iter, script);
						}
						else
						{
							++iter;
						}

					}
					for (auto& [name, offset] : mapTmp)
					{
						viewer.emplace(std::move(name), rsc::SharedFile::Iterator(script.getSize(), script.getData(), offset));
					}
				});
		}

		void loadScene(rsc::SharedFile::Iterator);

		void addLog(const SceneView& sceneView)
		{
			logs.emplace(sceneView.sceneName, sceneView);
		}
	};

	void SkipSpaceUntil(rsc::SharedFile::Iterator& ptr, unsigned char stop) { while (!ptr.eof() && *ptr != stop && std::isspace(*ptr)) ++ptr; }
	double ParsePrimary(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseTerm(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseExpression(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
}*/