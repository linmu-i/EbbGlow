#include <EbbGlow/VisualNovel/ScriptLoader/SceneFunctions.h>
#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/VisualNovel/VisualNovel/ColorTween.h>
#include <EbbGlow/VisualNovel/VisualNovel/ButtonAttachment.h>
#include <EbbGlow/VisualNovel/UI/BackLog.h>
#include <EbbGlow/VisualNovel/UI/SLControl.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/UI/UI.h>

namespace ebbglow::visualnovel
{
	void Scene_TextScene(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 6) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		auto& mainLibRsc = GetMainLibRsc();
		auto& virtualScreen = cfg.virtualScreen;

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

		//auto readIt = cfg.readTextSet.find(loader->sceneName);

		loader->world.createUnit(textBoxId, vn::MainTextBoxCom
		(
			Vec2{ virtualScreen.width * 0.1666667f, virtualScreen.height * 0.75f },
			virtualScreen.width * 0.6666667f,
			GetString(args[cfg.i18n.mainLanguage], *loader),
			GetString(args[cfg.i18n.secondaryLanguage], *loader),
			cfg.text.fontData, cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio,
			cfg.text.textSize * 0.3f * virtualScreen.drawRatio, cfg.text.textSpeed,
			&(loader->tmpLayers)[mainLibRsc.LayerDefine.textBoxLayer],
			cfg,
			colors::White//cfg.showReadText && readIt != cfg.readTextSet.end() ? cfg.readTextColor : colors::White
		));

		rsc::SharedTexture2D backGround(reinterpret_cast<const char8_t*>(GetString(args[4], *loader).c_str()));
		float scaleX = static_cast<float>(virtualScreen.width) / static_cast<float>(backGround.width());
		float scaleY = static_cast<float>(virtualScreen.height) / static_cast<float>(backGround.height());

		float bgScale;
		Vec2 bgPosition;

		if (args[5] == "@Cover")
		{
			bgScale = std::max(scaleX, scaleY);
			if (scaleX < scaleY)
			{
				bgPosition = { (virtualScreen.width - backGround.width() * bgScale) / 2.0f, 0.0f };
			}
			else
			{
				bgPosition = { 0.0f, (virtualScreen.height - backGround.height() * bgScale) / 2.0f };
			}
		}
		else
		{
			bgScale = std::min(scaleX, scaleY);
			if (scaleX > scaleY)
			{
				bgPosition = { (virtualScreen.width - backGround.width() * bgScale) / 2.0f, 0.0f };
			}
			else
			{
				bgPosition = { 0.0f, (virtualScreen.height - backGround.height() * bgScale) / 2.0f };
			}
		}

		auto idMgr = world.getEntityManager();
		if (backGround.valid())
		{
			loader->idList.push_back(idMgr->getId());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ backGround, bgPosition, &loader->tmpLayers[mainLibRsc.LayerDefine.backGroundLayer], bgScale });
		}
		if (mainLibRsc.textBoxBackGround.valid())
		{
			float scale = std::max(float(virtualScreen.width) / mainLibRsc.textBoxBackGround.width(), float(virtualScreen.height * (0.25f + 0.03125f)) / mainLibRsc.textBoxBackGround.height());
			loader->idList.push_back(idMgr->getId());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ mainLibRsc.textBoxBackGround, Vec2{ (virtualScreen.width - mainLibRsc.textBoxBackGround.width() * scale) / 2, virtualScreen.height * (0.75f - 0.03125f)}, &loader->tmpLayers[mainLibRsc.LayerDefine.textBoxBackGroundLayer], scale });
		}
	}

	void Scene_Chr(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.empty()) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		auto& mainLibRsc = GetMainLibRsc();
		auto& virtualScreen = cfg.virtualScreen;

		float offsetX = virtualScreen.width * (1.0f / 12.0f);
		float offsetY = virtualScreen.height * 0.0625f;
		float x = offsetX * 1.5f;
		float y = virtualScreen.height * 0.75f - offsetY;
		float textOffsetX = mainLibRsc.chrNameOffsetX * offsetX;
		if (mainLibRsc.chrNameBackGround.valid())
		{
			loader->idList.push_back(world.getEntityManager()->getId());
			float scale = static_cast<float>(offsetY) / static_cast<float>(mainLibRsc.chrNameBackGround.height());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ mainLibRsc.chrNameBackGround, Vec2{ static_cast<float>(x), static_cast<float>(y) }, &loader->tmpLayers[mainLibRsc.LayerDefine.textBoxLayer], scale});
		}

		loader->idList.push_back(world.getEntityManager()->getId());
		world.createUnit(loader->idList.back(), ui::TextBoxExCom{ cfg.text.fontData, GetString(args[0], *loader), Vec2{float(x + textOffsetX), float(y)}, cfg.text.textSize * virtualScreen.drawRatio, 0.1f * cfg.text.textSize * virtualScreen.drawRatio, colors::White, &loader->tmpLayers[mainLibRsc.LayerDefine.textBoxLayer] });
	}

	void Scene_Button(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 16) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		auto& mainLibRsc = GetMainLibRsc();
		auto& virtualScreen = cfg.virtualScreen;

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

		float offsetX = virtualScreen.width * relativeX, offsetY = virtualScreen.height * relativeY;
		float height = width / ratio;
		if (args[8] == "@Center")
		{
			offsetX -= (width * virtualScreen.width) / 2.0f;
			offsetY -= (height * virtualScreen.height) / 2.0f;
		}

		core::entity buttonId = world.getEntityManager()->getId();
		loader->idList.push_back(buttonId);

		auto text = GetString(args[static_cast<size_t>(4) + cfg.i18n.uiLanguage], *loader);
		world.createUnit(buttonId, ui::ButtonExCom
			{
				Rect{ Vec2{ offsetX, offsetY } + virtualScreen.drawOffset, Vec2{ width * virtualScreen.width, height * virtualScreen.width } },
				Rect{ Vec2{ offsetX, offsetY }, Vec2{ width * virtualScreen.width, height * virtualScreen.width }},
				& loader->tmpLayers[mainLibRsc.LayerDefine.ButtonLayer],
				normalImg,
				hoverImg,
				pressImg,
				utils::DynamicLoadFont(cfg.text.fontData, text, cfg.text.textSize),
				text,
				textColor,
				cfg.text.textSize * virtualScreen.drawRatio,
				cfg.text.textSize * 0.1f * virtualScreen.drawRatio
			});
		world.getMessageManager()->subscribe(buttonId);
	}

	void Scene_ImageBox(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 7) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		auto& mainLibRsc = GetMainLibRsc();
		auto& virtualScreen = cfg.virtualScreen;

		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float ratio = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float width = static_cast<float>(GetNumber(args[3], '\0', *loader));
		rsc::SharedTexture2D img(reinterpret_cast<const char8_t*>(GetString(args[4], *loader).c_str()));
		
		float scale;
		if (args[5] == "@Cover")
		{
			scale = std::max(width * virtualScreen.width / img.width(), width / ratio * virtualScreen.width / img.height());
		}
		else
		{
			scale = std::min(width * virtualScreen.width / img.width(), width / ratio * virtualScreen.width / img.height());
		}
		float x, y;
		if (args[6] == "@Center")
		{
			x = (relativeX * virtualScreen.width - img.width() * scale / 2);
			y = (relativeY * virtualScreen.height - img.height() * scale / 2);
		}
		else
		{
			x = relativeX * virtualScreen.width;
			y = relativeY * virtualScreen.height;
		}
		loader->idList.push_back(world.getEntityManager()->getId());
		world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ img, Vec2{x, y}, &loader->tmpLayers[mainLibRsc.LayerDefine.backGroundLayer], scale });
	}

	void Scene_SetStr(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 2) return;
		std::string* var = GetTextVariable(args[0], *loader);
		if (var == nullptr) return;
		*var = GetString(args[1], *loader);
	}

	void Scene_SetNum(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 2) return;
		if (args[0].empty()) return;
		int32_t offset = 0;
		size_t posOfIndex = args[0].find_last_of('[');
		if (args[0].back() == ']')
		{
			std::string offsetText = args[0].substr(posOfIndex + 1, args[0].length() - posOfIndex - 2);
			offset = static_cast<int32_t>(round(GetNumber(offsetText, '\0', *loader)));
		}
		double* var = GetNumberVariable(args[0], *loader);
		if (var == nullptr) return;
		*var = GetNumber(args[1], '\0', * loader);
	}

	struct KeyFrameAnimationBufCom
	{
		core::entity id = core::InvalidEntity;
		ui::KeyFramesAnimationCom com;
	};

	static KeyFrameAnimationBufCom& KeyframeAnimBuffer()
	{
		static thread_local KeyFrameAnimationBufCom buf;
		return buf;
	}

	void Scene_BeginKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 3) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		auto& virtualScreen = cfg.virtualScreen;

		rsc::SharedTexture2D img(reinterpret_cast<const char8_t*>(GetString(args[0], *loader).c_str()));
		float scale = virtualScreen.width / 1920.0f;
		uint8_t layerDepth = static_cast<uint8_t>(GetNumber(args[1], '\0', *loader));
		bool isLoop = args[2] == "@Loop";
		KeyframeAnimBuffer().id = world.getEntityManager()->getId();
		KeyframeAnimBuffer().com = ui::KeyFramesAnimationCom{ img, &loader->tmpLayers, scale, layerDepth, isLoop };
	}

	void Scene_AddKeyFrame(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 9) return;
		auto& world = loader->world;
		auto& animationActive = KeyframeAnimBuffer().com;
		auto& virtualScreen = loader->cfg.virtualScreen;

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
			keyFrame.position = Vec2{ relativeX * virtualScreen.width - animationActive.texture.width() * animationActive.scale / 2.0f * scale, relativeY * virtualScreen.height - animationActive.texture.height() * animationActive.scale / 2.0f * scale };
		}
		else
		{
			keyFrame.position = Vec2{ relativeX * virtualScreen.width, relativeY * virtualScreen.height };
		}
		KeyframeAnimBuffer().com.keyFrames.push_back(keyFrame);
	}

	void Scene_EndKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		auto& buf = KeyframeAnimBuffer();
		loader->idList.push_back(buf.id);
		loader->world.createUnit(buf.id, buf.com);
		buf.id = core::InvalidEntity;
		buf.com = ui::KeyFramesAnimationCom();
	}

	void Scene_SetBgm(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		std::string path = GetString(args[0], *loader);
		uint16_t volumeIndex = static_cast<uint16_t>(round(GetNumber(args[1], '\0', *loader)));
		float volume = 1.0f;
		if (volumeIndex < loader->cfg.audio.volumes.size())
		{
			volume = loader->cfg.audio.volumes[volumeIndex];
		}
		loader->musicMgr.SetBgm(rsc::SharedMusic(reinterpret_cast<const char8_t*>(path.c_str())), volume, path, volumeIndex);
	}

	void Scene_SetVoice(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		std::string path = GetString(args[0], *loader);
		uint16_t volumeIndex = static_cast<uint16_t>(round(GetNumber(args[1], '\0', *loader)));
		float volume = 1.0f;
		if (volumeIndex < loader->cfg.audio.volumes.size())
		{
			volume = loader->cfg.audio.volumes[volumeIndex];
		}
		loader->musicMgr.SetVoice(rsc::SharedSound(reinterpret_cast<const char8_t*>(path.c_str())), volume, path, volumeIndex);
	}

	void Scene_ColorTween(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		core::entity id = loader->world.getEntityManager()->getId();
		ColorR8G8B8A8 fromColor =
		{
			static_cast<uint8_t>(round(GetNumber(args[0], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[1], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[2], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[3], '\0', *loader)))
		};
		ColorR8G8B8A8 toColor =
		{
			static_cast<uint8_t>(round(GetNumber(args[4], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[5], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[6], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[7], '\0', *loader)))
		};
		float duration = static_cast<float>(GetNumber(args[8], '\0', *loader));
		uint8_t layerDepth = static_cast<uint8_t>(GetNumber(args[9], '\0', *loader));

		loader->idList.push_back(id);
		loader->world.createUnit(id, vn::ColorTweenCom{ fromColor, toColor, duration, &((loader->tmpLayers)[layerDepth])});
	}

	void Scene_BackLogCom(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 1) return;
		core::entity id = loader->world.getEntityManager()->getId();
		loader->idList.push_back(id);
		loader->world.createUnit(id, BackLogCom{ loader->cfg, &(loader->tmpLayers)[10], GetString(args[0], *loader), *loader});
	}

	void Scene_TextBoxCom(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 3) return;
		auto& mainLibRsc = GetMainLibRsc();
		auto& cfg = loader->cfg;
		auto& virtualScreen = cfg.virtualScreen;

		core::entity id = loader->world.getEntityManager()->getId();
		loader->idList.push_back(id);
		
		std::string text = GetString(args[0], *loader);
		float offsetX = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float offsetY = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float textSize = {};
		if (args.size() >= 3) textSize = static_cast<float>(GetNumber(args[3], '\0', *loader)) * virtualScreen.drawRatio;
		else textSize = cfg.text.textSize * virtualScreen.drawRatio;
		offsetX *= virtualScreen.width;
		offsetY *= virtualScreen.height;
		auto font = utils::DynamicLoadFont(cfg.text.fontData, text, textSize * virtualScreen.drawRatio);
		Vec2 offset = utils::MeasureTextSize(font, text, textSize * virtualScreen.drawRatio, 0.1f * cfg.text.textSize * virtualScreen.drawRatio);
		loader->world.createUnit(id,
			ui::TextBoxExCom
			{ 
				cfg.text.fontData, text,
				Vec2{ offsetX - offset.x / 2.0f, offsetY - offset.y / 2.0f },
				textSize * virtualScreen.drawRatio, 0.1f * textSize * virtualScreen.drawRatio, colors::White,
				&loader->tmpLayers[mainLibRsc.LayerDefine.textBoxLayer]
			});
	}

	void Scene_ButtonWithAttachment(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 16) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		auto& mainLibRsc = GetMainLibRsc();
		auto& virtualScreen = cfg.virtualScreen;

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

		float offsetX = virtualScreen.width * relativeX, offsetY = virtualScreen.height * relativeY;
		float height = width / ratio;
		if (args[8] == "@Center")
		{
			offsetX -= (width * virtualScreen.width) / 2.0f;
			offsetY -= (height * virtualScreen.height) / 2.0f;
		}

		core::entity buttonId = world.getEntityManager()->getId();
		loader->idList.push_back(buttonId);

		auto text = GetString(args[static_cast<size_t>(4) + cfg.i18n.uiLanguage], *loader);
		world.createUnit(
			buttonId,
			ui::ButtonExCom
			{
				Rect{ Vec2{ offsetX, offsetY } + virtualScreen.drawOffset, Vec2{ width * virtualScreen.width, height * virtualScreen.width } },
				Rect{ Vec2{ offsetX, offsetY }, Vec2{ width * virtualScreen.width, height * virtualScreen.width } },
				&loader->tmpLayers[mainLibRsc.LayerDefine.ButtonLayer],
				normalImg,
				hoverImg,
				pressImg,
				utils::DynamicLoadFont(cfg.text.fontData, text, cfg.text.textSize),
				text,
				textColor,
				cfg.text.textSize * virtualScreen.drawRatio,
				cfg.text.textSize * 0.1f * virtualScreen.drawRatio
			});
		for (int i = 16; i < args.size(); ++i)
		{
			auto cmd = Tokenizer(args[i]);
			CreateButtonAttachment(*loader, buttonId, cmd);
		}
		world.getMessageManager()->subscribe(buttonId);
	}

	void Scene_SaveBlocks(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		auto* sys = loader->world.getSystem<SLControlSystem>();
		if (!sys) return;
		sys->registerUpdateInfoList();
		sys->registerBuildSave();
		loader->idList.insert(loader->idList.end(), sys->getIdList().begin(), sys->getIdList().end());
	}

	void Scene_LoadBlocks(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		auto* sys = loader->world.getSystem<SLControlSystem>();
		if (!sys) return;
		sys->registerUpdateInfoList();
		sys->registerBuildLoad();
		loader->idList.insert(loader->idList.end(), sys->getIdList().begin(), sys->getIdList().end());
	}

	void Scene_SetBackLog(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (args.size() < 4) return;
		loader->backLogTmp.sceneName = loader->sceneName;
		if (args[0] != "@NotSet") loader->backLogTmp.text = args[0];
		if (args[1] != "@NotSet") loader->backLogTmp.exText = args[1];
		if (args[2] != "@NotSet") loader->backLogTmp.attText = args[2];
		if (args[3] != "@NotSet") loader->backLogTmp.voice = args[3];
	}

	void Scene_PushBackLog(ScriptLoader* loader, const std::vector<std::string>& args)
	{
		if (loader->backLogTmp.sceneName != loader->sceneName) loader->addToBackLog(loader->backLogTmp);
	}
}