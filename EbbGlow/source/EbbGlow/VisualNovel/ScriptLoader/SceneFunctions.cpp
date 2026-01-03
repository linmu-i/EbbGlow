#include <EbbGlow/VisualNovel/ScriptLoader/SceneFunctions.h>
#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/UI/UI.h>

namespace ebbglow::visualnovel
{
	void Scene_TextScene(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 6) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		std::vector<std::string> texts;
		texts.reserve(4);
		for (int i = 0; i < 4; ++i)
		{
			texts.push_back(args[i]);
		}

		core::entity textBoxId = loader->world.getEntityManager()->getId();
		if (loader->sceneType == "TextScene")
		{
			loader->exIdList.push_back(textBoxId);
		}
		else
		{
			loader->idList.push_back(textBoxId);
		}

		auto readIt = cfg.readTextSet.find(loader->sceneName);

		loader->world.createUnit(textBoxId, vn::MainTextBoxCom
		(
			GetString(args[cfg.mainLanguage], *loader),
			GetString(args[cfg.secondaryLanguage], *loader),
			cfg.textSize, cfg.fontData, cfg.textSpeed, Vec2({ cfg.ScreenWidth * 0.1666667f, cfg.ScreenHeight * 0.75f }) + cfg.drawOffset, cfg.ScreenWidth * 0.6666667f,
			cfg.showReadText && readIt != cfg.readTextSet.end() ? cfg.readTextColor : colors::White
		));

		rsc::SharedTexture2D backGround(reinterpret_cast<const char8_t*>(GetString(args[4], *loader).c_str()));
		float scaleX = static_cast<float>(cfg.ScreenWidth) / static_cast<float>(backGround.width());
		float scaleY = static_cast<float>(cfg.ScreenHeight) / static_cast<float>(backGround.height());

		float bgScale;
		Vec2 bgPosition;

		if (args[5] == "@Cover")
		{
			bgScale = std::max(scaleX, scaleY);
			if (scaleX < scaleY)
			{
				bgPosition = { (cfg.ScreenWidth - backGround.width() * bgScale) / 2.0f, 0.0f };
			}
			else
			{
				bgPosition = { 0.0f, (cfg.ScreenHeight - backGround.height() * bgScale) / 2.0f };
			}
		}
		else
		{
			bgScale = std::min(scaleX, scaleY);
			if (scaleX > scaleY)
			{
				bgPosition = { (cfg.ScreenWidth - backGround.width() * bgScale) / 2.0f, 0.0f };
			}
			else
			{
				bgPosition = { 0.0f, (cfg.ScreenHeight - backGround.height() * bgScale) / 2.0f };
			}
		}

		auto idMgr = world.getEntityManager();
		if (backGround.valid())
		{
			loader->idList.push_back(idMgr->getId());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom({ backGround, bgPosition + cfg.drawOffset, cfg.LayerDefine.backGroundLayer, world.getUiLayer(), bgScale }));
		}
		if (cfg.textBoxBackGround.valid())
		{
			float scale = std::max(float(cfg.ScreenWidth) / cfg.textBoxBackGround.width(), float(cfg.ScreenHeight * (0.25f + 0.03125f)) / cfg.textBoxBackGround.height());
			loader->idList.push_back(idMgr->getId());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom({ cfg.textBoxBackGround, Vec2{ (cfg.ScreenWidth - cfg.textBoxBackGround.width() * scale) / 2, cfg.ScreenHeight * (0.75f - 0.03125f)} + cfg.drawOffset, cfg.LayerDefine.textBoxBackGroundLayer, world.getUiLayer(), scale }));
		}
	}

	void Scene_Chr(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.empty()) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;

		float offsetX = cfg.ScreenWidth * (1.0f / 12.0f);
		float offsetY = cfg.ScreenHeight * 0.0625;
		float x = offsetX * 1.5f;
		float y = cfg.ScreenHeight * 0.75f - offsetY;
		float textOffsetX = cfg.chrNameOffsetX * offsetX;
		if (cfg.chrNameBackGround.valid())
		{
			loader->idList.push_back(world.getEntityManager()->getId());
			float scale = static_cast<float>(offsetY) / static_cast<float>(cfg.chrNameBackGround.height());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom({ cfg.chrNameBackGround, Vec2{ static_cast<float>(x), static_cast<float>(y) } + cfg.drawOffset, cfg.LayerDefine.textBoxLayer, loader->world.getUiLayer(), scale}));
		}

		loader->idList.push_back(world.getEntityManager()->getId());
		world.createUnit(loader->idList.back(), ui::TextBoxExCom{ cfg.fontData, GetString(args[0], *loader), Vec2{float(x + textOffsetX), float(y)} + cfg.drawOffset, float(cfg.textSize), 0.1f * cfg.textSize, colors::White, loader->world.getUiLayer(), cfg.LayerDefine.textBoxLayer });
	}

	void Scene_Button(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 16) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;

		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float ratio = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float width = static_cast<float>(GetNumber(args[3], '\0', *loader));
		rsc::SharedTexture2D normalImg(reinterpret_cast<const char8_t*>(GetString(args[13], *loader).c_str()));
		rsc::SharedTexture2D hoverImg(reinterpret_cast<const char8_t*>(GetString(args[14], *loader).c_str()));
		rsc::SharedTexture2D pressImg(reinterpret_cast<const char8_t*>(GetString(args[15], *loader).c_str()));
		ColorR8G8B8A8 textColor = 
		{ 
			static_cast<uint8_t>(round(GetNumber(args[9], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[10], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[11], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[12], '\0', *loader)))
		};

		float offsetX = cfg.ScreenWidth * relativeX, offsetY = cfg.ScreenHeight * relativeY;
		float height = width / ratio;
		if (args[8] == "@Center")
		{
			offsetX -= (width * cfg.ScreenWidth) / 2.0f;
			offsetY -= (height * cfg.ScreenHeight) / 2.0f;
		}

		core::entity buttonId = world.getEntityManager()->getId();
		if (loader->sceneType == "SelectScene")
		{
			loader->exIdList.push_back(buttonId);
		}
		else
		{
			loader->idList.push_back(buttonId);
		}
		auto text = GetString(args[4 + cfg.uiLanguage], *loader);
		world.createUnit(buttonId, ui::ButtonExCom
			{
				Rect{ Vec2{ offsetX, offsetY } + cfg.drawOffset, Vec2{ width * cfg.ScreenWidth, height * cfg.ScreenWidth } },
				cfg.LayerDefine.ButtonLayer,
				world.getUiLayer(),
				normalImg,
				hoverImg,
				pressImg,
				(width * cfg.ScreenWidth) / normalImg.width(),
				utils::DynamicLoadFont(cfg.fontData, text, cfg.textSize),
				text,
				textColor,
				static_cast<float>(cfg.textSize),
				cfg.textSize * 0.1f
			});
		world.getMessageManager()->subscribe(buttonId);
	}

	void Scene_ImageBox(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 7) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float ratio = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float width = static_cast<float>(GetNumber(args[3], '\0', *loader));
		rsc::SharedTexture2D img(reinterpret_cast<const char8_t*>(GetString(args[4], *loader).c_str()));
		float scale;
		if (args[5] == "@Cover")
		{
			scale = std::max(width * cfg.ScreenWidth / img.width(), width / ratio * cfg.ScreenWidth / img.height());
		}
		else
		{
			scale = std::min(width * cfg.ScreenWidth / img.width(), width / ratio * cfg.ScreenWidth / img.height());
		}
		float x, y;
		if (args[6] == "@Center")
		{
			x = (relativeX * cfg.ScreenWidth - img.width() * scale / 2);
			y = (relativeY * cfg.ScreenHeight - img.height() * scale / 2);
		}
		else
		{
			x = relativeX * cfg.ScreenWidth;
			y = relativeY * cfg.ScreenHeight;
		}
		loader->idList.push_back(world.getEntityManager()->getId());
		world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ img, Vec2{x, y} + cfg.drawOffset, cfg.LayerDefine.backGroundLayer, world.getUiLayer(), scale });
	}

	void Scene_SetStr(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 2) return;
		std::string* var = GetTextVariable(args[0], *loader);
		if (var == nullptr) return;
		*var = GetString(args[1], *loader);
	}

	void Scene_SetNum(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 2) return;
		double* var = GetNumberVariable(args[0], *loader);
		if (var == nullptr) return;
		*var = GetNumber(args[1], '\0', * loader);
	}

	struct KeyFrameAnimationBufCom
	{
		core::entity id = 0;
		ui::KeyFramesAnimationCom com;
	};

	static KeyFrameAnimationBufCom& KeyframeAnimBuffer()
	{
		static thread_local KeyFrameAnimationBufCom buf;
		return buf;
	}

	void Scene_BeginKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 3) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		rsc::SharedTexture2D img(reinterpret_cast<const char8_t*>(GetString(args[0], *loader).c_str()));
		float scale = cfg.ScreenWidth / 1920.0f;
		uint8_t layerDepth = static_cast<uint8_t>(GetNumber(args[1], '\0', *loader));
		bool isLoop = args[2] == "@Loop";
		KeyframeAnimBuffer().id = world.getEntityManager()->getId();
		KeyframeAnimBuffer().com = ui::KeyFramesAnimationCom{ img, world.getUiLayer(), scale, layerDepth, isLoop };
		//loader->idList.push_back(world.getEntityManager()->getId());
		//world.createUnit(loader->idList.back(), ui::KeyFramesAnimationCom{ img, world.getUiLayer(), scale, layerDepth, isLoop });
	}

	void Scene_AddKeyFrame(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 9) return;
		auto& world = loader->world;
		auto& animationActive = KeyframeAnimBuffer().com;
		ui::KeyFrame keyFrame;
		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float originX = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float originY = static_cast<float>(GetNumber(args[3], '\0', *loader));
		float scale = static_cast<float>(GetNumber(args[4], '\0', *loader));
		float rotation = static_cast<float>(GetNumber(args[5], '\0', *loader));
		float duration = static_cast<float>(GetNumber(args[6], '\0', *loader));
		uint8_t alpha = static_cast<uint8_t>(round(GetNumber(args[7], '\0', *loader)));
		keyFrame.origin = Vec2{ originX * animationActive.texture.width(), originY * animationActive.texture.height() };
		keyFrame.duration = duration;
		keyFrame.rotation = rotation;
		keyFrame.scale = scale;
		keyFrame.alpha = alpha;
		if (args[8] == "@Center")
		{
			keyFrame.position = Vec2{ relativeX * loader->cfg.ScreenWidth - animationActive.texture.width() * animationActive.scale / 2.0f * scale, relativeY * loader->cfg.ScreenHeight - animationActive.texture.height() * animationActive.scale / 2.0f * scale };
		}
		else
		{
			keyFrame.position = Vec2{ relativeX * loader->cfg.ScreenWidth, relativeY * loader->cfg.ScreenHeight };
		}
		KeyframeAnimBuffer().com.keyFrames.push_back(keyFrame);
	}

	void Scene_EndKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		auto& buf = KeyframeAnimBuffer();
		loader->idList.push_back(buf.id);
		loader->world.createUnit(buf.id, buf.com);
		buf.id = 0;
		buf.com = ui::KeyFramesAnimationCom();
	}

	void Scene_SetBgm(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		loader->musicMgr.SetBgm(rsc::SharedMusic(GetString(args[0], *loader).c_str()), loader->cfg.volumes[static_cast<size_t>(round(GetNumber(args[1], '\0', *loader)))]);
	}

	void Scene_SetVoice(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		loader->musicMgr.SetVoice(rsc::SharedSound(reinterpret_cast<const char8_t*>(GetString(args[0], *loader).c_str())), loader->cfg.volumes[static_cast<size_t>(round(GetNumber(args[1], '\0', *loader)))]);
	}
}



/*
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
	Vec2 bgPosition;
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
}*/