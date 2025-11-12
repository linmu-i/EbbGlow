#pragma once

#include "ECS.h"
#include "World.h"
#include "raylibRAII.h"
#include "Message.h"
#include "RLUtils.h"


namespace ui
{
	class ButtonPressMsg : public ecs::MessageBase
	{
	public:
		ButtonPressMsg(ecs::entity senderId, ecs::MessageTypeId typeId) : MessageBase(senderId, typeId) {}
	};

	struct ButtonCom
	{
		float x;
		float y;

		float width;
		float height;

		int fontSize;

		Color textColor;

		rlRAII::Texture2DRAII icon;
		Color color;

		bool isIcon;

		rlRAII::FontRAII font;

		std::string text;

		ButtonCom(const Vector2 pos, const float width, const float height, const int fontSize, const Color textColor, const char* icoPath, std::string text, const char* fontPath) :
			x(pos.x), y(pos.y), width(width), height(height), fontSize(fontSize), isIcon(true), textColor(textColor), text(text), font(fontPath), color({0})
		{
			if (icoPath != nullptr)
			{
				icon = rlRAII::Texture2DRAII(icoPath);
			}
		}

		ButtonCom(const Vector2 pos, const float width, const float height, const int fontSize, const Color textColor, const char* icoPath, std::string text, Font font) :
			x(pos.x), y(pos.y), width(width), height(height), fontSize(fontSize), isIcon(true), textColor(textColor), text(text), font(font), color({ 0 })
		{
			if (icoPath != nullptr)
			{
				icon = rlRAII::Texture2DRAII(icoPath);
			}
		}

		ButtonCom(const Vector2 pos, const float width, const float height, const int fontSize, const Color textColor, const Color color, std::string text, const char* fontPath) :
			x(pos.x), y(pos.y), width(width), height(height), fontSize(fontSize), color(color), isIcon(false), textColor(textColor), text(text)
		{
			if (fontPath != nullptr)
			{
				font = rlRAII::FontRAII(fontPath);
			}
		}
		ButtonCom(const Vector2 pos, const float width, const float height, const int fontSize, const Color textColor, const Color color, std::string text, rlRAII::FontRAII font) :
			x(pos.x), y(pos.y), width(width), height(height), fontSize(fontSize), color(color), isIcon(false), textColor(textColor), text(text), font(font) {}


		~ButtonCom() {}
	};

	Vector2 TextCenteredOffset(const Font& font, const std::string& text, float fontSize, float spacing, Vector2 coverage)
	{
		Vector2 size = MeasureTextEx(font, text.c_str(), fontSize, spacing);
		float offsetY = (coverage.y - size.y) / 2;
		float offsetX = (coverage.x - size.x) / 2;
		return { offsetX, offsetY };
	}

	class ButtonDraw : public ecs::DrawBase
	{
	private:
		Color color;
		Color textColor;
		rlRAII::Texture2DRAII icon;

		Vector2 pos;
		Vector2 covered;
		rlRAII::FontRAII font;
		std::string text;
		bool isIcon;
		int fontSize;

	public:
		ButtonDraw(const ButtonCom& button) : pos({ button.x, button.y }), covered({ button.width, button.height }), font(button.font), isIcon(button.isIcon), text(button.text), fontSize(button.fontSize), textColor(button.textColor), color({ 0 })
		{
			if (button.isIcon)
			{
				icon = button.icon;
			}
			else
			{
				color = button.color;
			}
		}

		void draw()
		{
			Vector2 textOffset;
			if (font.valid())
			{
				textOffset = TextCenteredOffset(font.get(), text, fontSize, 0, covered);
			}
			else
			{
				textOffset = TextCenteredOffset(GetFontDefault(), text, fontSize, 0, covered);
			}

			if (isIcon)
			{
				DrawTexture(icon.get(), pos.x, pos.y, WHITE);
			}
			else
			{
				DrawRectangle(pos.x, pos.y, covered.x, covered.y, color);
			}
			if (font.valid())
			{
				DrawSDFText(font.get(), text.c_str(), pos + textOffset, fontSize, 0, textColor);
			}
			else
			{
				DrawText(text.c_str(), pos.x + textOffset.x, pos.y + textOffset.y, fontSize, textColor);
			}

		}
	};

	class ButtonSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<ButtonCom>& buttons;
		ecs::MessageManager& msgmgr;
		std::array<std::vector<std::unique_ptr<ecs::DrawBase>>, 16>& uiLayer;

		std::shared_ptr<std::vector<ecs::entity>> buttonIds;
		std::shared_ptr<std::vector<std::vector<ecs::entity>>> listeners;

		ecs::MessageTypeId messageTypeId;

	public:
		ButtonSystem(ecs::DoubleComs<ButtonCom>* b, ecs::MessageManager* m, std::array<std::vector<std::unique_ptr<ecs::DrawBase>>, 16>* uiLayer) : buttons(*b), msgmgr(*m), uiLayer(*uiLayer)
		{
			buttonIds = std::make_shared<std::vector<ecs::entity>>();
			listeners = std::make_shared<std::vector<std::vector<ecs::entity>>>();

			messageTypeId = m->getMessageTypeManager().registeredType<ButtonPressMsg>();
		}


		void registerListener(ecs::entity buttonId, ecs::entity listenerId)
		{
			auto it = std::find(buttonIds.get()->begin(), buttonIds.get()->end(), buttonId);
			if (it == buttonIds.get()->end())
			{
				buttonIds.get()->push_back(buttonId);
				listeners.get()->push_back(std::vector<ecs::entity>());
				it = std::find(buttonIds.get()->begin(), buttonIds.get()->end(), buttonId);
			}
			(*listeners)[it - buttonIds.get()->begin()].push_back(listenerId);
		}

		ecs::MessageTypeId typeId() const//获取本系统下的按钮消息id
		{
			return messageTypeId;
		}

		void update()//发送按下消息
		{
			auto release = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
			Vector2 pos;
			if (release)
			{
				pos = GetMousePosition();
			}
			buttons.active()->forEach([release, pos, this](ecs::entity id, ButtonCom& button)
				{
					if (release)
					{
						if (pos.x < button.x + button.width && pos.x > button.x && pos.y < button.y + button.height && pos.y > button.y)
						{
							auto it = std::find(buttonIds.get()->begin(), buttonIds.get()->end(), id);//查找对应id的监听列表
							//msgmgr.addMessage(ButtonPressMsg(messageSysId, id));
							if (it != buttonIds.get()->end())
							{
								msgmgr.addMulticastMessage(std::make_unique<ButtonPressMsg>(ButtonPressMsg(id, messageTypeId)), (*listeners)[it - buttonIds.get()->begin()]);
							}
						}
					}
					uiLayer[10].push_back(std::make_unique<ButtonDraw>(button));
					int i = 1;
				});

		}
	};

	void ApplyButton(ecs::World2D& world)
	{
		world.addPool<ButtonCom>();
		world.addSystem(std::move(ButtonSystem(world.getDoubleBuffer<ButtonCom>(), world.getMessageManager(), world.getUiLayer())));
	}

	struct ImageBoxCom
	{
		Vector2 pos;
		rlRAII::Texture2DRAII img;
	};

	class ImageBoxDraw : public ecs::DrawBase
	{
	private:
		ImageBoxCom imageBox;

	public:
		ImageBoxDraw(ImageBoxCom& imageBox) : imageBox(imageBox) {}

		void draw() override
		{
			DrawTexture(imageBox.img.get(), imageBox.pos.x, imageBox.pos.y, WHITE);
		}
	};

	class ImageBoxSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<ImageBoxCom>& imageBoxes;
		ecs::Layers& layers;

	public:
		ImageBoxSystem(ecs::DoubleComs<ImageBoxCom>* imageBoxes, ecs::Layers* layers) : imageBoxes(*imageBoxes), layers(*layers) {}
		void update()
		{
			imageBoxes.active()->forEach
			(
				[this](ecs::entity id, ImageBoxCom& imgBox)
				{
					layers[0].push_back(std::make_unique<ImageBoxDraw>(ImageBoxDraw(imgBox)));
				}
			);
		}
	};

	void ApplyImageBox(ecs::World2D& world)
	{
		world.addPool<ImageBoxCom>();
		world.addSystem(std::move(ImageBoxSystem(world.getDoubleBuffer<ImageBoxCom>(), world.getUnitsLayer())));
	}

	struct TextBoxCom
	{
		Vector2 pos;
		rlRAII::FontRAII font;
		std::string text;
		Color textColor;
		int fontSize;
		float spacing;
	};

	class TextBoxDraw : public ecs::DrawBase
	{
	private:
		TextBoxCom textBox;

	public:
		TextBoxDraw(TextBoxCom& textBox) : textBox(textBox) {}
		void draw() override
		{
			if (textBox.font.valid())
			{
				DrawSDFText(textBox.font.get(), textBox.text.c_str(), textBox.pos, textBox.fontSize, textBox.spacing, textBox.textColor);
			}
			else
			{
				DrawText(textBox.text.c_str(), textBox.pos.x, textBox.pos.y, textBox.fontSize, textBox.textColor);
			}
		}
	};

	class TextBoxSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<TextBoxCom>& textBoxes;
		ecs::Layers& layer;

	public:
		TextBoxSystem(ecs::DoubleComs<TextBoxCom>* textBoxes, ecs::Layers* layer) : textBoxes(*textBoxes), layer(*layer) {}

		void update() override
		{
			textBoxes.active()->forEach
			(
				[this](ecs::entity id, TextBoxCom& textBox)
				{
					layer[9].push_back(std::make_unique<TextBoxDraw>(textBox));
				}
			);
		}
	};

	void ApplyTextBox(ecs::World2D& world)
	{
		world.addPool<TextBoxCom>();
		world.addSystem(std::move(TextBoxSystem(world.getDoubleBuffer<TextBoxCom>(), world.getUnitsLayer())));
	}

	class ButtonExCom
	{
	private:
		friend class ButtonExDraw;
		friend class ButtonExSystem;
		rlRAII::FontRAII font;
		std::string fontPath;

		std::string text;
		int textSize;

	public:
		rlRAII::Texture2DRAII baseIcon;
		rlRAII::Texture2DRAII hoverIcon;
		rlRAII::Texture2DRAII pressIcon;

		Color textColor;
		int spacing;

		Vector2 pos;
		Vector2 coverage;

		uint8_t layerDepth;

		bool press;

		float iconScale;

		ButtonExCom
		(
			rlRAII::FileRAII fontData,
			rlRAII::Texture2DRAII baseIcon,
			rlRAII::Texture2DRAII hoverIcon,
			rlRAII::Texture2DRAII pressIcon,

			std::string text,
			Color textColor,
			int textSize,
			int spacing,

			Vector2 pos,
			Vector2 coverage,
			uint8_t layerDepth,
			float iconScale
		) : fontPath(fontPath), baseIcon(baseIcon), hoverIcon(hoverIcon), pressIcon(pressIcon), text(text), textColor(textColor),
			textSize(textSize), spacing(spacing), pos(pos), coverage(coverage), layerDepth(layerDepth), press(false), iconScale(iconScale)
		{
			font = rlRAII::FontRAII(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), textSize * 2.0f));
		}

		void resetText(const char* newText, rlRAII::FileRAII newFont, float newSize)
		{
			text = newText;
			textSize = newSize;
			font = rlRAII::FontRAII(DynamicLoadFontFromMemory(newText, newFont.fileName(), newFont.get(), newFont.size(), newSize * 2.0f));
		}
	};

	class ButtonExDraw : public ecs::DrawBase
	{
	private:
		rlRAII::FontRAII font;
		rlRAII::Texture2DRAII icon;
		std::string text;
		Color textColor;
		int fontSize;
		int spacing;
		Vector2 pos;
		Vector2 coverage;
		float iconScale;

	public:
		ButtonExDraw(const ButtonExCom button, rlRAII::TextureRAII icon) :
			font(button.font), icon(icon), text(button.text), textColor(button.textColor), fontSize(button.textSize),
			spacing(button.spacing), pos(button.pos), coverage(button.coverage), iconScale(button.iconScale) { }

		void draw() override
		{
			DrawTextureEx(icon.get(), pos, 0, iconScale, WHITE);
			if (text.length() > 0)
			{
				Vector2 offset = TextCenteredOffset(font.get(), text, fontSize, spacing, coverage);
				//DrawSDFText(font.get(), text.c_str(), (pos + offset), fontSize, spacing, textColor);
				DrawTextEx(font.get(), text.c_str(), (pos + offset), fontSize, spacing, textColor);
			}
		}
	};

	class ButtonExSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<ButtonExCom>& buttons;
		ecs::Layers& uiLayer;
		ecs::MessageManager& msgMgr;

		ecs::MessageTypeId messageTypeId;

		rlRAII::RenderTexture2DRAII rt;

	public:
		ButtonExSystem(ecs::DoubleComs<ButtonExCom>* buttons, ecs::Layers* uiLayer, ecs::MessageManager* msgMgr) : buttons(*buttons), uiLayer(*uiLayer), msgMgr(*msgMgr)
		{
			messageTypeId = msgMgr->getMessageTypeManager().registeredType<ButtonPressMsg>();
		}

		void update() override
		{
			buttons.active()->forEach
			(
				[this](ecs::entity id, ButtonExCom& button)
				{
					Vector2 mPos = GetMousePosition();
					bool inBox = mPos.x < button.pos.x + button.coverage.x && mPos.x > button.pos.x && mPos.y < button.pos.y + button.coverage.y && mPos.y > button.pos.y;
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						if (inBox && button.press == false)
						{
							button.press = true;
						}
					}
					if (button.press)
					{
						if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
						{
							if (inBox)
							{
								button.press = false;
								msgMgr.addUnicastMessage(std::make_unique<ButtonPressMsg>(ButtonPressMsg(id, messageTypeId)), id);
							}
							else
							{
								button.press = false;
							}
						}
					}
					if (inBox)
					{
						if (button.press)
						{
							uiLayer[button.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(button, button.pressIcon)));
						}
						else
						{
							uiLayer[button.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(button, button.hoverIcon)));
						}
					}
					else
					{
						uiLayer[button.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(button, button.baseIcon)));
					}
				}
			);

			buttons.inactive()->forEach
			(
				[this](ecs::entity id, ButtonExCom& button)
				{
					Vector2 mPos = GetMousePosition();
					bool inBox = mPos.x < button.pos.x + button.coverage.x && mPos.x > button.pos.x && mPos.y < button.pos.y + button.coverage.y && mPos.y > button.pos.y;
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						if (inBox && button.press == false)
						{
							button.press = true;
						}
					}
					if (button.press)
					{
						if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
						{
							if (inBox)
							{
								button.press = false;
								msgMgr.addUnicastMessage(std::make_unique<ButtonPressMsg>(ButtonPressMsg(id, messageTypeId)), id);
							}
							else
							{
								button.press = false;
							}
						}
					}
					if (inBox)
					{
						if (button.press)
						{
							uiLayer[button.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(button, button.pressIcon)));
						}
						else
						{
							uiLayer[button.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(button, button.hoverIcon)));
						}
					}
					else
					{
						uiLayer[button.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(button, button.baseIcon)));
					}
				}
			);
		}

	};

	void ApplyButtonEx(ecs::World2D& world)
	{
		world.addPool<ButtonExCom>();
		world.addSystem(ButtonExSystem(world.getDoubleBuffer<ButtonExCom>(), world.getUiLayer(), world.getMessageManager()));
	}

	class TextBoxExCom
	{
	private:
		friend class TextBoxExDraw;
		friend class TextBoxExSystem;
		std::string text;

		rlRAII::FontRAII font;
		float fontSize;
		rlRAII::FileRAII fontData;
		//std::string fontPath;

		bool initialized;

	public:
		Vector2 position;
		int layerDepth;

		Color textColor;
		float spacing;
		float rotation;
		
		TextBoxExCom
		(
			rlRAII::FileRAII fontData,
			std::string text,
			Vector2 position,
			Color textColor,
			int layerDepth,
			float fontSize,
			float spacing,
			float rotation = 0
		) : fontData(fontData), text(text), position(position), textColor(textColor), fontSize(fontSize), spacing(spacing), rotation(rotation), layerDepth(layerDepth), initialized(false)
		{
			font = rlRAII::FontRAII(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), fontSize));
		}

		void resetText(const char* newText)
		{
			text = newText;
			font = rlRAII::FontRAII(DynamicLoadFontFromMemory(newText, fontData.fileName(), fontData.get(), fontData.size(), fontSize));
		}
		void resetFont(rlRAII::FileRAII newFont)
		{
			fontData = newFont;
			font = rlRAII::FontRAII(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), fontSize));
		}
		void resetFontSize(float newFontSize)
		{
			fontSize = newFontSize;
			font = rlRAII::FontRAII(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), fontSize));
		}

		void unload()
		{
			font.~FontRAII();
			initialized = false;
		}

		void load()
		{
			if (!initialized)
			{
				font = rlRAII::FontRAII(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), fontSize));
			}
		}
	};

	class TextBoxExDraw : public ecs::DrawBase
	{
	private:
		std::string text;

		rlRAII::FontRAII font;
		float fontSize;
		Vector2 position;

		Color textColor;
		float spacing;
		float rotation;

	public:
		TextBoxExDraw(TextBoxExCom& textBoxEx) : 
			text(textBoxEx.text), font(textBoxEx.font), fontSize(textBoxEx.fontSize), 
			position(textBoxEx.position), textColor(textBoxEx.textColor), rotation(textBoxEx.rotation), spacing(textBoxEx.spacing)
		{}

		void draw() override
		{
			DrawTextPro(font.get(), text.c_str(), position, { 0,0 }, rotation, fontSize, spacing, textColor);
			//DrawTextureEx(font.get().texture, { 0,0 }, 0, 0.5, WHITE);
		}
	};

	class TextBoxExSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<TextBoxExCom>& textBoxesEx;
		ecs::Layers& layer;

	public:
		TextBoxExSystem(ecs::DoubleComs<TextBoxExCom>* textBoxes, ecs::Layers* layer) : textBoxesEx(*textBoxes), layer(*layer) {}

		void update() override
		{
			textBoxesEx.active()->forEach
			(
				[this](ecs::entity id, TextBoxExCom& textBox)
				{
					layer[9].push_back(std::make_unique<TextBoxExDraw>(textBox));
				}
			);
		}
	};

	void ApplyTextBoxEx(ecs::World2D& world)
	{
		world.addPool<TextBoxExCom>();
		world.addSystem(std::move(TextBoxExSystem(world.getDoubleBuffer<TextBoxExCom>(), world.getUiLayer())));
	}

	struct SwitchCom
	{
		bool state;
		bool press;
		float radius;
		Color color;
		Vector2 pos;
		float s;
		float scale;
		int layerDepth;

		SwitchCom(float radius, Color color, Vector2 position, int layerDepth) :
			state(false), press(false), radius(radius), color(color), pos(position), s(0.0f), scale(0.0f), layerDepth(layerDepth) {}
	};

	class SwitchDraw : public ecs::DrawBase
	{
	private:
		SwitchCom swt;

	public:
		SwitchDraw(SwitchCom switchCom) : swt(switchCom) {}

		void draw() override
		{
			Vector3 hsv = ColorToHSV(swt.color);
			DrawCircleV(swt.pos, swt.radius, ColorFromHSV(hsv.x, swt.s * hsv.y, hsv.z));
			DrawCircleV(swt.pos, swt.radius * 0.75f, WHITE);
			DrawCircleV(swt.pos, swt.radius * 0.5f * swt.scale, swt.color);
		}
	};

	class SwitchSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<SwitchCom>* coms;
		ecs::Layers* layers;

	public:
		SwitchSystem(ecs::DoubleComs<SwitchCom>* coms, ecs::Layers* layers) : coms(coms), layers(layers) {}

		void update() override
		{
			coms->active()->forEach
			(
				[this](ecs::entity id, SwitchCom& comActive)
				{
					auto& comInactive = *(coms->inactive()->get(id));
					float deltaTime = GetFrameTime();
					comInactive = comActive;
					if (Vector2DistanceSqr(GetMousePosition(), comActive.pos) > comActive.radius * comActive.radius)
					{
						comInactive.s -= deltaTime * 6.0f;
						comInactive.s = std::clamp(comInactive.s, 0.5f, 1.0f);
						if (comActive.press && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
						{
							comInactive.press = false;
						}
					}
					else
					{
						comInactive.s += deltaTime * 6.0f;
						comInactive.s = std::clamp(comInactive.s, 0.0f, 1.0f);
						if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && comActive.press)
						{
							comInactive.press = false;
							comInactive.state = !comActive.state;
						}
						else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !comActive.press)
						{
							comInactive.press = true;
						}

					}
					if (comActive.state)
					{
						comInactive.scale += deltaTime * 20.0f;
						comInactive.scale = std::clamp(comInactive.scale, 0.0f, 1.0f);
					}
					else
					{
						comInactive.scale -= deltaTime * 20.0f;
						comInactive.scale = std::clamp(comInactive.scale, 0.0f, 1.0f);
					}
					(*layers)[comActive.layerDepth].push_back(std::make_unique<SwitchDraw>(SwitchDraw(comActive)));
				}
			);
		}
	};

	void ApplySwitch(ecs::World2D& world)
	{
		world.addPool<SwitchCom>();
		world.addSystem(SwitchSystem(world.getDoubleBuffer<SwitchCom>(), world.getUiLayer()));
	}

	struct SliderCom
	{
		float value;
		float length;
		float scale;

		float pressCount;
		float hoverCount;

		bool press;

		int graduation;
		int layerDepth;

		Vector2 pos;
		Color trackColor;
		Color thumbColor;

		rlRAII::Texture2DRAII thumbTexture;
		rlRAII::Texture2DRAII trackTexture;

		SliderCom(float length, float value, float scale, int graduation, Vector2 pos, Color trackColor, Color thumbColor, rlRAII::Texture2DRAII thumbTexture, rlRAII::Texture2DRAII trackTexture, int layerDepth) :
			length(length), graduation(graduation), pos(pos), trackColor(trackColor), thumbColor(thumbColor), thumbTexture(thumbTexture),
			trackTexture(trackTexture), layerDepth(layerDepth), value(value), pressCount(0.0f), hoverCount(0.0f), press(false), scale(scale)
		{}
	};

	class SliderDraw : public ecs::DrawBase
	{
	private:
		SliderCom slider;
		float halfL;
		float delta;

	public:
		SliderDraw(SliderCom slider, float halfL, float delta) : slider(slider), halfL(halfL), delta(delta) {}

		void draw() override
		{
			//float halfL = slider.length * 0.5;
			if (slider.trackTexture.valid())
			{
				DrawTextureEx(slider.trackTexture.get(), { slider.pos.x - slider.trackTexture.get().width * 0.5f, slider.pos.y - slider.trackTexture.get().height * 0.5f }, 0.0f, slider.scale, WHITE);
			}
			else
			{
				DrawLineEx({ slider.pos.x - halfL, slider.pos.y }, { slider.pos.x + halfL, slider.pos.y }, 10 * slider.scale, slider.trackColor);
				if (slider.graduation > 1)
				{
					//float delta = slider.length / (slider.graduation - 1);
					for (int i = 0; i < slider.graduation; ++i)
					{
						DrawLineEx({ slider.pos.x - halfL + delta * i, slider.pos.y - 10 }, { slider.pos.x - halfL + delta * i, slider.pos.y + 10 }, 5 * slider.scale, slider.trackColor);
					}
				}
			}
			if (slider.thumbTexture.valid())
			{
				Vector2 pos = { slider.pos.x - halfL + slider.length * slider.value - slider.thumbTexture.get().width * 0.5f, slider.pos.y - slider.thumbTexture.get().height * 0.5f };
				DrawTextureEx(slider.thumbTexture.get(), pos, 0.0f, slider.scale, WHITE);
			}
			else
			{
				Vector3 hsv = ColorToHSV(slider.thumbColor);
				Vector2 pos = { slider.pos.x - halfL + slider.length * slider.value, slider.pos.y};
				DrawCircleV(pos, 15.0f * slider.scale, ColorFromHSV(hsv.x, slider.hoverCount * hsv.y, hsv.z));
				DrawCircleV(pos, 11.25f * slider.scale, WHITE);
				DrawCircleV(pos, 7.5f * slider.pressCount, slider.thumbColor);
			}
		}
	};

	class SliderSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<SliderCom>* coms;
		ecs::Layers* layers;

	public:
		SliderSystem(ecs::DoubleComs<SliderCom>* coms, ecs::Layers* layers) : coms(coms), layers(layers) {}

		void update() override
		{
			coms->active()->forEach
			(
				[this](ecs::entity id, SliderCom& comActive)
				{
					SliderCom& comInactive = *(coms->inactive()->get(id));

					comInactive = comActive;

					float deltaTime = GetFrameTime();
					float halfL = comActive.length * 0.5f;
					float delta = comActive.length / (comActive.graduation - 1);
					Vector2 thumbPos = { comActive.pos.x - halfL + comActive.length * comActive.value, comActive.pos.y };

					if (comActive.press && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
					{
						comInactive.press = false;
					}
					if (Vector2DistanceSqr(thumbPos, GetMousePosition()) > 225 * comActive.scale * comActive.scale)
					{
						comInactive.hoverCount -= deltaTime * 6.0f;
						comInactive.hoverCount = std::clamp(comInactive.hoverCount, 0.5f, 1.0f);
					}
					else
					{
						comInactive.hoverCount += deltaTime * 6.0f;
						comInactive.hoverCount = std::clamp(comInactive.hoverCount, 0.5f, 1.0f);
						if (!comActive.press && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
						{
							comInactive.press = true;
						}
					}
					if (comActive.press)
					{
						comInactive.pressCount += deltaTime * 20.0f;
						comInactive.pressCount = std::clamp(comInactive.pressCount, 0.0f, 1.0f);
						float x = GetMousePosition().x;
						x = std::clamp(x, comActive.pos.x - halfL, comActive.pos.x + halfL);
						x -= comActive.pos.x;
						x += halfL;
						if (comActive.graduation > 1)
						{
							comInactive.value = std::clamp(int(x / delta + 0.5f) / float(comActive.graduation - 1), 0.0f, 1.0f);
						}
						else
						{
							comInactive.value = x / comActive.length;
						}
					}
					else
					{
						comInactive.pressCount -= deltaTime * 20.0f;
						comInactive.pressCount = std::clamp(comInactive.pressCount, 0.0f, 1.0f);
					}
					(*layers)[comActive.layerDepth].push_back(std::make_unique<SliderDraw>(SliderDraw(comActive, halfL, delta)));
				}
			);
		}
	};

	void ApplySlider(ecs::World2D& world)
	{
		world.addPool<SliderCom>();
		world.addSystem(SliderSystem(world.getDoubleBuffer<SliderCom>(), world.getUiLayer()));
	}

	struct ImageBoxExCom
	{
		Vector2 pos;
		float scale;
		rlRAII::Texture2DRAII img;
		int layerDepth;
	};

	class ImageBoxExDraw : public ecs::DrawBase
	{
	private:
		Vector2 pos;
		float scale;
		rlRAII::Texture2DRAII img;

	public:
		ImageBoxExDraw(const Vector2 pos, const float scale, const rlRAII::Texture2DRAII& img) : pos(pos), scale(scale), img(img) {}

		void draw() override
		{
			DrawTextureEx(img.get(), pos, 0.0f, scale, WHITE);
		}
	};

	class ImageBoxExSystem : public ecs::SystemBase
	{
		private:
		ecs::DoubleComs<ImageBoxExCom>& imageBoxes;
		ecs::Layers& layers;

	public:
		ImageBoxExSystem(ecs::DoubleComs<ImageBoxExCom>* imageBoxes, ecs::Layers* layers) : imageBoxes(*imageBoxes), layers(*layers) {}
		void update()
		{
			imageBoxes.active()->forEach
			(
				[this](ecs::entity id, ImageBoxExCom& imgBox)
				{
					layers[imgBox.layerDepth].push_back(std::make_unique<ImageBoxExDraw>(ImageBoxExDraw{ imgBox.pos, imgBox.scale, imgBox.img }));
				}
			);
		}
	};

	void ApplyImageBoxEx(ecs::World2D& world)
	{
		world.addPool<ImageBoxExCom>();
		world.addSystem(ImageBoxExSystem(world.getDoubleBuffer<ImageBoxExCom>(), world.getUnitsLayer()));
	}
}