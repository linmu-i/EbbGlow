#pragma once

#include <fstream>
#include <future>

#include <core/ECS.h>
#include <core/World.h>
#include "VisualNovel.h"

namespace visualnovel
{
	class ScriptData
	{
	private:
		unsigned char* data;
		size_t size;

	public:
		ScriptData() : data(nullptr), size(0) {}
		ScriptData(const char* filePath)
		{
			std::ifstream file(filePath, std::ios::binary | std::ios::ate);
			if (file)
			{
				size = file.tellg();
				data = new unsigned char[size];
				file.seekg(0, std::ios::beg);
				file.read(reinterpret_cast<char*>(data), size);
				file.close();
			}
			else
			{
				data = nullptr;
				size = 0;
			}
		}
		ScriptData(const ScriptData&) = delete;
		ScriptData(ScriptData&& other) noexcept : data(other.data), size(other.size)
		{
			other.data = nullptr;
			other.size = 0;
		}

		~ScriptData()
		{
			delete[] data;
		}

		
		ScriptData& operator=(const ScriptData&) = delete;

		ScriptData& operator=(ScriptData&& other) noexcept
		{
			if (this != &other)
			{
				delete[] data;
				data = other.data;
				size = other.size;
				other.data = nullptr;
				other.size = 0;
			}
			return *this;
		}

		ScriptData& operator+=(const ScriptData& other)
		{
			unsigned char* newData = new unsigned char[size + other.size + 1];
			memcpy(newData, data, size);
			newData[size] = '\n';
			memcpy(newData + size + 1, other.data, other.size);
			delete[] data;
			data = newData;
			size += (other.size + 1);
			return *this;
		}

		ScriptData& operator+=(const rsc::SharedFile& other)
		{
			unsigned char* newData = new unsigned char[size + other.size() + 1];
			memcpy(newData, data, size);
			newData[size] = '\n';
			memcpy(newData + size + 1, other.get(), other.size());
			delete[] data;
			data = newData;
			size += (other.size() + 1);
			return *this;
		}

		rsc::SharedFile::Iterator begin()
		{
			return rsc::SharedFile::Iterator(size, data);
		}
		rsc::SharedFile::Iterator end()
		{
			return rsc::SharedFile::Iterator(size, data, size);
		}

		const unsigned char* getData() const { return data; }
		size_t getSize() const { return size; }
	};

	class MusicManager : public ecs::SystemBase
	{
	private:
		VisualNovelConfig cfg;

		rsc::SharedMusic bgm;
		rsc::SharedSound voice;

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
			enum class SceneType : uint8_t
			{
				TYPE_TEXTSCENE = 0,
				TYPE_SELECTSCENE = 1
			};
			std::string sceneName;
			SceneType sceneType;
			std::string text;
		};

	private:
		ecs::World2D& world;
		ScriptData script;
		visualnovel::VisualNovelConfig& cfg;

		std::unordered_map<std::string, rsc::SharedFile::Iterator> viewer;
		std::unordered_map<std::string, SceneView> logs;
		std::vector<std::string> targetScene;
		std::vector<ecs::entity> idList;
		std::vector<ecs::entity> exIdList;

		std::vector<std::string> StringStorage;
		std::vector<double> NumberStorage;

		std::string beginScene;

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
		std::unordered_map<std::string, VariableView> variables;

		

		void CreateTextScene(const std::vector<std::string>& languages, ecs::entity textBoxId, rsc::SharedTexture2D backGround, bool read, BackgroundDrawType drawType)
		{
			world.createUnit(textBoxId, vn::StandardTextBox
			(
				languages[cfg.mainLanguage],
				languages[cfg.secondaryLanguage],
				cfg.textSize, cfg.fontData, cfg.textSpeed, Vector2({ cfg.ScreenWidth * 0.1666667f, cfg.ScreenHeight * 0.75f}) + cfg.drawOffset, cfg.ScreenWidth * 0.6666667f,
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
				world.createUnit(idList.back(), ui::ImageBoxExCom({ Vector2{ (cfg.ScreenWidth - cfg.textBoxBackGround.get().width * scale) / 2, cfg.ScreenHeight * (0.75f - 0.03125f)} + cfg.drawOffset, scale, cfg.textBoxBackGround, cfg.LayerDefine.textBoxBackGroundLayer}));
			}
		}

		void SkipSpaceUntil(rsc::SharedFile::Iterator& ptr, unsigned char stop)
		{
			while (!ptr.eof() && *ptr != stop && std::isspace(*ptr))
				++ptr;
		}

		double f0(rsc::SharedFile::Iterator& ptr, unsigned char stop)
		{
			SkipSpaceUntil(ptr, stop);
			if (ptr.eof() || *ptr == stop) return 0.0;

			char c = *ptr;

			if (c == '$')
			{
				++ptr;
				std::string varName;
				while (!ptr.eof() && *ptr != stop && (std::isalpha(static_cast<unsigned char>(*ptr)) || *ptr == '_' || isdigit(*ptr)))
				{
					varName += *ptr;
					++ptr;
				}

				int32_t index = 0;
				if (!ptr.eof() && *ptr != stop && *ptr == '[')
				{
					++ptr;
					SkipSpaceUntil(ptr, stop);
					if (ptr.eof() || *ptr == stop) return 0.0;
					index = (int)round(f2(ptr, ']'));

					SkipSpaceUntil(ptr, stop);
					if (!ptr.eof() && *ptr != stop && *ptr == ']')
						++ptr;
				}

				auto varView = GetVariable(varName);
				if (varView.length == 0)
					return 0.0;
				if (varView.type == VarType::Num)
					return NumberStorage[varView.offset + index];
				return 0.0;
			}

			if (c == '+')
			{
				++ptr;
				return +f0(ptr, stop);
			}
			if (c == '-')
			{
				++ptr;
				return -f0(ptr, stop);
			}

			if (std::isdigit(static_cast<unsigned char>(c)) || c == '.')
			{
				std::string numberBuf;
				while (!ptr.eof() && *ptr != stop &&
					(std::isdigit(static_cast<unsigned char>(*ptr)) || *ptr == '.'))
				{
					if (*ptr == '.' && numberBuf.find('.') != std::string::npos) break;
					numberBuf += *ptr;
					++ptr;
				}
				if (numberBuf.empty()) return 0.0;
				return std::stod(numberBuf);
			}

			if (c == '(')
			{
				++ptr;
				double val = f2(ptr, stop);
				SkipSpaceUntil(ptr, stop);
				if (!ptr.eof() && *ptr != stop && *ptr == ')')
					++ptr;
				return val;
			}

			return 0.0;
		}
		double f1(rsc::SharedFile::Iterator& ptr, unsigned char stop)
		{
			double result = f0(ptr, stop);
			SkipSpaceUntil(ptr, stop);

			while (!ptr.eof() && *ptr != stop)
			{
				char op = *ptr;
				if (op == '*' || op == '/' || op == '%')
				{
					++ptr;
					SkipSpaceUntil(ptr, stop);
					double rhs = (ptr.eof() || *ptr == stop) ? 0.0 : f0(ptr, stop);
					if (op == '*')
						result *= rhs;
					else if (op == '/')
						result = (rhs == 0.0) ? std::numeric_limits<double>::quiet_NaN() : result / rhs;
					else if (op == '%')
						result = std::fmod(result, rhs);
					SkipSpaceUntil(ptr, stop);
				}
				else
					break;
			}
			return result;
		}
		double f2(rsc::SharedFile::Iterator& ptr, unsigned char stop)
		{
			double result = f1(ptr, stop);
			SkipSpaceUntil(ptr, stop);

			while (!ptr.eof() && *ptr != stop)
			{
				char op = *ptr;
				if (op == '+' || op == '-')
				{
					++ptr;
					SkipSpaceUntil(ptr, stop);
					double rhs = (ptr.eof() || *ptr == stop) ? 0.0 : f1(ptr, stop);
					if (op == '+')
						result += rhs;
					else
						result -= rhs;
					SkipSpaceUntil(ptr, stop);
				}
				else
					break;
			}
			return result;
		}

		void ReadNextNumber(std::string& textBuf, rsc::SharedFile::Iterator& nextScene)
		{
			textBuf.clear();
			while (!isdigit(*nextScene) && *nextScene != '.' && *nextScene != '+' && *nextScene != '-' && !nextScene.eof()) ++nextScene;
			while ((isdigit(*nextScene) || *nextScene == '.' || *nextScene == '+' || *nextScene == '-') && !nextScene.eof())
			{
				textBuf += *nextScene;
				++nextScene;
			}
			while ((*nextScene != ',' && *nextScene != ')') && !nextScene.eof()) ++nextScene;
			++nextScene;
		}
		void ReadNextStateTag(std::string& textBuf, rsc::SharedFile::Iterator& nextScene)
		{
			textBuf.clear();
			while (*nextScene != '@' && !nextScene.eof()) ++nextScene;
			++nextScene;
			while (isspace(*nextScene) && !nextScene.eof()) ++nextScene;
			while (!isspace(*nextScene) && *nextScene != ',' && *nextScene != ')' && !nextScene.eof())
			{
				textBuf += *nextScene;
				++nextScene;
			}
			while ((*nextScene != ',' && *nextScene != ')') && !nextScene.eof()) ++nextScene;
			++nextScene;
		}
		void ReadNextString(std::string& textBuf, rsc::SharedFile::Iterator& nextScene)
		{
			textBuf.clear();
			while (*nextScene != '"' && *nextScene != '$' && !nextScene.eof()) ++nextScene;
			if (*nextScene == '$')
			{
				++nextScene;
				std::string varName;
				while (!isspace(*nextScene) && *nextScene != '[' && !nextScene.eof())
				{
					varName += *nextScene;
					++nextScene;
				}
				int32_t index = 0;
				if (*nextScene == '[')
				{
					++nextScene;
					std::string indexBuf;
					while (*nextScene != ']' && !nextScene.eof())
					{
						indexBuf += *nextScene;
						++nextScene;
					}
					if (!indexBuf.empty())
						index = static_cast<int32_t>(std::stoi(indexBuf));
					if (*nextScene == ']')
						++nextScene;
				}
				auto varView = GetVariable(varName);
				if (varView.length != 0 && varView.type == VarType::Str)
				{
					if (index >= 0 && index < varView.length)
					{
						textBuf = StringStorage[varView.offset + index];
					}
				}
				else
				{
					textBuf = "";
				}
				return;
			}
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

		std::string GetString(rsc::SharedFile::Iterator& nextScene)
		{
			std::string result;
			std::string buf;
			while (*nextScene != ')' && *nextScene != ',' && !nextScene.eof())
			{
				ReadNextString(buf, nextScene);
				result += buf;
			}
			++nextScene;
			return result;
		}
		double GetNumber(rsc::SharedFile::Iterator& nextScene, unsigned char stop)
		{
			double result = f2(nextScene, stop);
			while ((*nextScene != ',' && *nextScene != ')') && !nextScene.eof()) ++nextScene;
			++nextScene;
			return result;
		}
		VariableView GetVariable(const std::string& name)
		{
			auto it = variables.find(name);
			if (it != variables.end())
			{
				return it->second;
			}
			else
			{
				return VariableView{ VarType::Num, 0, 0 };
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
					CreateTextScene(texts, exIdList.back(), rsc::SharedTexture2D(bg.c_str()), readIt == cfg.readTextSet.end(), bgType);
				}
				else
				{
					auto idmgr = world.getEntityManager();
					idList.push_back(idmgr->getId());
					CreateTextScene(texts, idList.back(), rsc::SharedTexture2D(bg.c_str()), readIt == cfg.readTextSet.end(), bgType);
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

				ReadNextNumber(buf, nextScene);
				textColor.r = std::stoi(buf);
				ReadNextNumber(buf, nextScene);
				textColor.g = std::stoi(buf);
				ReadNextNumber(buf, nextScene);
				textColor.b = std::stoi(buf);
				ReadNextNumber(buf, nextScene);
				textColor.a = std::stoi(buf);

				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				normalImg = rsc::SharedTexture2D(LoadTexture(buf.c_str()));
				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				hoverImg = rsc::SharedTexture2D(LoadTexture(buf.c_str()));
				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				pressImg = rsc::SharedTexture2D(LoadTexture(buf.c_str()));

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
				buf = GetString(nextScene);//ReadNextString(buf, nextScene);
				img = rsc::SharedTexture2D(LoadTexture(buf.c_str()));
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

	public:
		ScriptLoader(ecs::World2D& world, const std::string& script, vn::VisualNovelConfig& cfg) : world(world), script(script.c_str()), cfg(cfg) {}

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
			ScriptData scriptDataTmp;
			if (memcmp(scriptPtr.get(), "Global", 6)) return std::async(std::launch::deferred, []() {});
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
				else
				{
					++scriptPtr;
				}
			}
			script += scriptDataTmp;
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

		void loadScene(rsc::SharedFile::Iterator);

		void addLog(const SceneView& sceneView)
		{
			logs.emplace(sceneView.sceneName, sceneView);
		}
	};

	struct TextSceneCom
	{
		ecs::entity txtBoxId;
		rsc::SharedFile::Iterator nextScene;
		ScriptLoader::SceneView view;
	};

	class TextSceneSystem : public ecs::SystemBase
	{
	private:
		ecs::World2D* world;
		ecs::DoubleComs<TextSceneCom>* coms;
		ecs::DoubleComs<ui::ButtonExCom>* buttonComs;
		ecs::DoubleComs<StandardTextBox>* textBoxComs;
		ScriptLoader* scLoader;
		std::string nextSceneName;
		bool clicked = false;

		ecs::MessageTypeId pressMsgId;

	public:
		TextSceneSystem(ecs::World2D& world, ScriptLoader& scLoader) :
			world(&world), coms(world.getDoubleBuffer<TextSceneCom>()), buttonComs(world.getDoubleBuffer<ui::ButtonExCom>()),
			clicked(false), textBoxComs(world.getDoubleBuffer<StandardTextBox>()), scLoader(&scLoader),
			pressMsgId(world.getMessageManager()->getMessageTypeManager().getId<ui::ButtonPressMsg>()){}

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
		std::vector<ecs::entity> buttonIdList;
		std::vector<rsc::SharedFile::Iterator> nextSceneList;
	};

	class SelectSceneSystem : public ecs::SystemBase
	{
	private:
		ecs::World2D* world;
		ecs::DoubleComs<SelectSceneCom>* coms;
		ScriptLoader* scLoader;

		ecs::MessageTypeId pressMsgId = world->getMessageManager()->getMessageTypeManager().getId<ui::ButtonReleaseMsg>();

	public:
		SelectSceneSystem(ecs::World2D& world, ScriptLoader& scLoader) :
			world(&world), coms(world.getDoubleBuffer<SelectSceneCom>()), scLoader(&scLoader) {}

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

	struct DelaySceneCom
	{
		double delay;
		double timeCount = 0;

		rsc::SharedFile::Iterator targetScene;
	};

	class DelaySceneSystem : public ecs::SystemBase
	{
	private:
		ecs::World2D* world;
		ecs::DoubleComs<DelaySceneCom>* coms;

		ScriptLoader* scLoader;

	public:
		DelaySceneSystem(ecs::World2D& world, ScriptLoader& scLoader) : world(&world), coms(world.getDoubleBuffer<DelaySceneCom>()), scLoader(&scLoader) {}

		void update()
		{
			coms->active()->forEach([this](ecs::entity id, DelaySceneCom& activeCom)
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

	void ApplyScriptLoader(ecs::World2D& world, ScriptLoader& scriptLoader, VisualNovelConfig& cfg)
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
	}
}