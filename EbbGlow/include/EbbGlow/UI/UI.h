#pragma once

#include <EbbGlow/UI/Animation.h>
#include <EbbGlow/UI/Button.h>
#include <EbbGlow/UI/ImageBox.h>
#include <EbbGlow/UI/TextBox.h>

/*
#include "EbbGlow/core/ECS.h"
#include "EbbGlow/core/World.h"
#include "EbbGlow/utils/Resource.h"
#include "EbbGlow/core/Message.h"
#include "EbbGlow/utils/RLUtils.h"
#include "EbbGlow/platform/Platform.h"

namespace ui
{

	

	struct ImageBoxCom
	{
		Vector2 pos;
		rsc::SharedTexture2D img;
	};

	class ImageBoxDraw : public core::DrawBase
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

	class ImageBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ImageBoxCom>& imageBoxes;
		core::Layers& layers;

	public:
		ImageBoxSystem(core::DoubleComs<ImageBoxCom>* imageBoxes, core::Layers* layers) : imageBoxes(*imageBoxes), layers(*layers) {}
		void update()
		{
			imageBoxes.active()->forEach
			(
				[this](core::entity id, ImageBoxCom& imgBox)
				{
					layers[0].push_back(std::make_unique<ImageBoxDraw>(ImageBoxDraw(imgBox)));
				}
			);
		}
	};

	void ApplyImageBox(core::World2D& world)
	{
		world.addPool<ImageBoxCom>();
		world.addSystem(std::move(ImageBoxSystem(world.getDoubleBuffer<ImageBoxCom>(), world.getUnitsLayer())));
	}

	struct TextBoxCom
	{
		Vector2 pos;
		rsc::SharedFont font;
		std::string text;
		Color textColor;
		int fontSize;
		float spacing;
	};

	class TextBoxDraw : public core::DrawBase
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

	class TextBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TextBoxCom>& textBoxes;
		core::Layers& layer;

	public:
		TextBoxSystem(core::DoubleComs<TextBoxCom>* textBoxes, core::Layers* layer) : textBoxes(*textBoxes), layer(*layer) {}

		void update() override
		{
			textBoxes.active()->forEach
			(
				[this](core::entity id, TextBoxCom& textBox)
				{
					layer[9].push_back(std::make_unique<TextBoxDraw>(textBox));
				}
			);
		}
	};

	void ApplyTextBox(core::World2D& world)
	{
		world.addPool<TextBoxCom>();
		world.addSystem(std::move(TextBoxSystem(world.getDoubleBuffer<TextBoxCom>(), world.getUnitsLayer())));
	}

	class ButtonExCom
	{
	private:
		friend class ButtonExDraw;
		friend class ButtonExSystem;
		rsc::SharedFont font;
		std::string fontPath;

		std::string text;
		int textSize;

	public:
		rsc::SharedTexture2D baseIcon;
		rsc::SharedTexture2D hoverIcon;
		rsc::SharedTexture2D pressIcon;

		Color textColor;
		int spacing;

		Vector2 pos;
		Vector2 coverage;

		uint8_t layerDepth;

		bool press;

		float iconScale;

		bool touch;

		ButtonExCom
		(
			rsc::SharedFile fontData,
			rsc::SharedTexture2D baseIcon,
			rsc::SharedTexture2D hoverIcon,
			rsc::SharedTexture2D pressIcon,

			std::string text,
			Color textColor,
			int textSize,
			int spacing,

			Vector2 pos,
			Vector2 coverage,
			uint8_t layerDepth,
			float iconScale
		) : fontPath(fontPath), baseIcon(baseIcon), hoverIcon(hoverIcon), pressIcon(pressIcon), text(text), textColor(textColor),
			textSize(textSize), spacing(spacing), pos(pos), coverage(coverage), layerDepth(layerDepth), press(false), iconScale(iconScale), touch(false)
		{
			font = rsc::SharedFont(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), textSize * 2.0f));
		}

		void resetText(const char* newText, rsc::SharedFile newFont, float newSize)
		{
			text = newText;
			textSize = newSize;
			font = rsc::SharedFont(DynamicLoadFontFromMemory(newText, newFont.fileName(), newFont.get(), newFont.size(), newSize * 2.0f));
		}
	};

	class ButtonExDraw : public core::DrawBase
	{
	private:
		rsc::SharedFont font;
		rsc::SharedTexture2D icon;
		std::string text;
		Color textColor;
		int fontSize;
		int spacing;
		Vector2 pos;
		Vector2 coverage;
		float iconScale;

	public:
		ButtonExDraw(const ButtonExCom button, rsc::SharedTexture icon) :
			font(button.font), icon(icon), text(button.text), textColor(button.textColor), fontSize(button.textSize),
			spacing(button.spacing), pos(button.pos), coverage(button.coverage), iconScale(button.iconScale) { }

		void draw() override
		{
			DrawTextureEx(icon.get(), pos, 0, iconScale, WHITE);
			if (text.length() > 0)
			{
				Vector2 offset = TextCenteredOffset(font.get(), text, fontSize, spacing, coverage);
				DrawTextEx(font.get(), text.c_str(), (pos + offset), fontSize, spacing, textColor);
			}
		}
	};

	class ButtonExSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ButtonExCom>& buttons;
		core::Layers& uiLayer;
		core::MessageManager& msgMgr;

		core::MessageTypeId pressTypeId;
		core::MessageTypeId releaseTypeId;

		rsc::SharedRenderTexture2D rt;

	public:
		ButtonExSystem(core::DoubleComs<ButtonExCom>* buttons, core::Layers* uiLayer, core::MessageManager* msgMgr) : buttons(*buttons), uiLayer(*uiLayer), msgMgr(*msgMgr)
		{
			pressTypeId = msgMgr->getMessageTypeManager().registeredType<ButtonPressMsg>();
			releaseTypeId = msgMgr->getMessageTypeManager().registeredType<ButtonReleaseMsg>();
		}

		void update() override
		{
			buttons.active()->forEach
			(
				[this](core::entity id, ButtonExCom& buttonActive)
				{
					auto& buttonInactive = *(buttons.inactive()->get(id));
					buttonInactive = buttonActive;
					Vector2 mPos = GetMousePosition();
					int touchCount = GetTouchPointCountEx();
					
					buttonInactive.touch = false;

					bool touchInbox = false;
					if (!CheckCollisionPointRec(mPos, { buttonActive.pos.x, buttonActive.pos.y, buttonActive.coverage.x, buttonActive.coverage.y }) && touchCount > 0)
					{
						for (int i = 0; i < touchCount; ++i)
						{
							auto touchPos = GetTouchPositionEx(i);
							if (CheckCollisionPointRec(touchPos, { buttonActive.pos.x, buttonActive.pos.y, buttonActive.coverage.x, buttonActive.coverage.y }))
							{
								mPos = touchPos;
								buttonInactive.touch = true;
								touchInbox = true;
								break;
							}
						}
					}
					if (buttonActive.touch && !buttonInactive.touch)
					{
						msgMgr.addUnicastMessage(std::make_unique<ButtonReleaseMsg>(ButtonReleaseMsg(id, releaseTypeId)), id);
					}
					if (!buttonActive.touch && buttonInactive.touch)
					{
						msgMgr.addUnicastMessage(std::make_unique<ButtonPressMsg>(ButtonPressMsg(id, pressTypeId)), id);
					}
					
					bool inBox = mPos.x < buttonActive.pos.x + buttonActive.coverage.x && mPos.x > buttonActive.pos.x && mPos.y < buttonActive.pos.y + buttonActive.coverage.y && mPos.y > buttonActive.pos.y;
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						if (inBox && buttonActive.press == false)
						{
							buttonInactive.press = true;
							msgMgr.addUnicastMessage(std::make_unique<ButtonPressMsg>(ButtonPressMsg(id, pressTypeId)), id);
						}
					}
					if (buttonActive.press)
					{
						if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
						{
							if (inBox)
							{
								buttonInactive.press = false;
								msgMgr.addUnicastMessage(std::make_unique<ButtonReleaseMsg>(ButtonReleaseMsg(id, releaseTypeId)), id);
							}
							else
							{
								buttonInactive.press = false;
							}
						}
					}
					if (inBox || touchInbox)
					{
						if (buttonActive.press || buttonActive.touch)
						{
							uiLayer[buttonActive.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(buttonActive, buttonActive.pressIcon)));
						}
						else
						{
							uiLayer[buttonActive.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(buttonActive, buttonActive.hoverIcon)));
						}
					}
					else
					{
						uiLayer[buttonActive.layerDepth].push_back(std::make_unique<ButtonExDraw>(ButtonExDraw(buttonActive, buttonActive.baseIcon)));
					}
				}
			);
		}

	};

	void ApplyButtonEx(core::World2D& world)
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

		rsc::SharedFont font;
		float textSize;
		rsc::SharedFile fontData;

		bool initialized;

	public:
		Vector2 position;
		int layerDepth;

		Color textColor;
		float spacing;
		float rotation;
		
		TextBoxExCom
		(
			rsc::SharedFile fontData,
			std::string text,
			Vector2 position,
			Color textColor,
			int layerDepth,
			float textSize,
			float spacing,
			float rotation = 0
		) : fontData(fontData), text(text), position(position), textColor(textColor), textSize(textSize), spacing(spacing), rotation(rotation), layerDepth(layerDepth), initialized(false)
		{
			font = rsc::SharedFont(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), textSize));
		}

		void resetText(const char* newText)
		{
			text = newText;
			font = rsc::SharedFont(DynamicLoadFontFromMemory(newText, fontData.fileName(), fontData.get(), fontData.size(), textSize * 2.0f));
		}
		void resetFont(rsc::SharedFile newFont)
		{
			fontData = newFont;
			font = rsc::SharedFont(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), textSize * 2.0f));
		}
		void resetFontSize(float newFontSize)
		{
			textSize = newFontSize;
			font = rsc::SharedFont(DynamicLoadFontFromMemory(text.c_str(), fontData.fileName(), fontData.get(), fontData.size(), textSize * 2.0f));
		}


	};

	class TextBoxExDraw : public core::DrawBase
	{
	private:
		std::string text;

		rsc::SharedFont font;
		float textSize;
		Vector2 position;

		Color textColor;
		float spacing;
		float rotation;

	public:
		TextBoxExDraw(TextBoxExCom& textBoxEx) : 
			text(textBoxEx.text), font(textBoxEx.font), textSize(textBoxEx.textSize),
			position(textBoxEx.position), textColor(textBoxEx.textColor), rotation(textBoxEx.rotation), spacing(textBoxEx.spacing)
		{}

		void draw() override
		{
			DrawTextPro(font.get(), text.c_str(), position, { 0,0 }, rotation, textSize, spacing, textColor);
			//DrawTextureEx(font.get().texture, { 0,0 }, 0, 0.5, WHITE);
		}
	};

	class TextBoxExSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<TextBoxExCom>& textBoxesEx;
		core::Layers& layer;

	public:
		TextBoxExSystem(core::DoubleComs<TextBoxExCom>* textBoxes, core::Layers* layer) : textBoxesEx(*textBoxes), layer(*layer) {}

		void update() override
		{
			textBoxesEx.active()->forEach
			(
				[this](core::entity id, TextBoxExCom& textBox)
				{
					layer[9].push_back(std::make_unique<TextBoxExDraw>(textBox));
				}
			);
		}
	};

	void ApplyTextBoxEx(core::World2D& world)
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

	class SwitchDraw : public core::DrawBase
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

	class SwitchSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<SwitchCom>* coms;
		core::Layers* layers;

	public:
		SwitchSystem(core::DoubleComs<SwitchCom>* coms, core::Layers* layers) : coms(coms), layers(layers) {}

		void update() override
		{
			coms->active()->forEach
			(
				[this](core::entity id, SwitchCom& comActive)
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

	void ApplySwitch(core::World2D& world)
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

		rsc::SharedTexture2D thumbTexture;
		rsc::SharedTexture2D trackTexture;

		SliderCom(float length, float value, float scale, int graduation, Vector2 pos, Color trackColor, Color thumbColor, rsc::SharedTexture2D thumbTexture, rsc::SharedTexture2D trackTexture, int layerDepth) :
			length(length), graduation(graduation), pos(pos), trackColor(trackColor), thumbColor(thumbColor), thumbTexture(thumbTexture),
			trackTexture(trackTexture), layerDepth(layerDepth), value(value), pressCount(0.0f), hoverCount(0.0f), press(false), scale(scale)
		{}
	};

	class SliderDraw : public core::DrawBase
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

	class SliderSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<SliderCom>* coms;
		core::Layers* layers;

	public:
		SliderSystem(core::DoubleComs<SliderCom>* coms, core::Layers* layers) : coms(coms), layers(layers) {}

		void update() override
		{
			coms->active()->forEach
			(
				[this](core::entity id, SliderCom& comActive)
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

	struct RefSliderCom
	{
		float& value;
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

		rsc::SharedTexture2D thumbTexture;
		rsc::SharedTexture2D trackTexture;

		RefSliderCom(float length, float& value, float scale, int graduation, Vector2 pos, Color trackColor, Color thumbColor, rsc::SharedTexture2D thumbTexture, rsc::SharedTexture2D trackTexture, int layerDepth) :
			length(length), graduation(graduation), pos(pos), trackColor(trackColor), thumbColor(thumbColor), thumbTexture(thumbTexture),
			trackTexture(trackTexture), layerDepth(layerDepth), value(value), pressCount(0.0f), hoverCount(0.0f), press(false), scale(scale)
		{}

		RefSliderCom& operator=(const RefSliderCom& other)
		{
			if (this != &other)
			{
				length = other.length;
				scale = other.scale;
				graduation = other.graduation;
				pos = other.pos;
				trackColor = other.trackColor;
				thumbColor = other.thumbColor;
				thumbTexture = other.thumbTexture;
				trackTexture = other.trackTexture;
				layerDepth = other.layerDepth;
				pressCount = other.pressCount;
				hoverCount = other.hoverCount;
				press = other.press;
			}
			return *this;
		}
	};

	class RefSliderDraw : public core::DrawBase
	{
	private:
		RefSliderCom slider;
		float halfL;
		float delta;

	public:
		RefSliderDraw(RefSliderCom slider, float halfL, float delta) : slider(slider), halfL(halfL), delta(delta) {}

		void draw() override
		{
			if (slider.trackTexture.valid())
			{
				DrawTextureEx(slider.trackTexture.get(), { slider.pos.x - slider.trackTexture.get().width * 0.5f, slider.pos.y - slider.trackTexture.get().height * 0.5f }, 0.0f, slider.scale, WHITE);
			}
			else
			{
				DrawLineEx({ slider.pos.x - halfL, slider.pos.y }, { slider.pos.x + halfL, slider.pos.y }, 10 * slider.scale, slider.trackColor);
				if (slider.graduation > 1)
				{
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
				Vector2 pos = { slider.pos.x - halfL + slider.length * slider.value, slider.pos.y };
				DrawCircleV(pos, 15.0f * slider.scale, ColorFromHSV(hsv.x, slider.hoverCount * hsv.y, hsv.z));
				DrawCircleV(pos, 11.25f * slider.scale, WHITE);
				DrawCircleV(pos, 7.5f * slider.pressCount, slider.thumbColor);
			}
		}
	};

	class RefSliderSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<RefSliderCom>* coms;
		core::Layers* layers;

	public:
		RefSliderSystem(core::DoubleComs<RefSliderCom>* coms, core::Layers* layers) : coms(coms), layers(layers) {}

		void update() override
		{
			coms->active()->forEach
			(
				[this](core::entity id, RefSliderCom& comActive)
				{
					RefSliderCom& comInactive = *(coms->inactive()->get(id));

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
					(*layers)[comActive.layerDepth].push_back(std::make_unique<RefSliderDraw>(RefSliderDraw(comActive, halfL, delta)));
				}
			);
		}
	};

	void ApplySlider(core::World2D& world)
	{
		world.addPool<SliderCom>();
		world.addPool<RefSliderCom>();
		world.addSystem(SliderSystem(world.getDoubleBuffer<SliderCom>(), world.getUiLayer()));
		world.addSystem(RefSliderSystem(world.getDoubleBuffer<RefSliderCom>(), world.getUiLayer()));
	}

	struct ImageBoxExCom
	{
		Vector2 pos;
		float scale;
		rsc::SharedTexture2D img;
		int layerDepth;
	};

	class ImageBoxExDraw : public core::DrawBase
	{
	private:
		Vector2 pos;
		float scale;
		rsc::SharedTexture2D img;

	public:
		ImageBoxExDraw(const Vector2 pos, const float scale, const rsc::SharedTexture2D& img) : pos(pos), scale(scale), img(img) {}

		void draw() override
		{
			DrawTextureEx(img.get(), pos, 0.0f, scale, WHITE);
		}
	};

	class ImageBoxExSystem : public core::SystemBase
	{
		private:
		core::DoubleComs<ImageBoxExCom>& imageBoxes;
		core::Layers& layers;

	public:
		ImageBoxExSystem(core::DoubleComs<ImageBoxExCom>* imageBoxes, core::Layers* layers) : imageBoxes(*imageBoxes), layers(*layers) {}
		void update()
		{
			imageBoxes.active()->forEach
			(
				[this](core::entity id, ImageBoxExCom& imgBox)
				{
					layers[imgBox.layerDepth].push_back(std::make_unique<ImageBoxExDraw>(ImageBoxExDraw{ imgBox.pos, imgBox.scale, imgBox.img }));
				}
			);
		}
	};

	void ApplyImageBoxEx(core::World2D& world)
	{
		world.addPool<ImageBoxExCom>();
		world.addSystem(ImageBoxExSystem(world.getDoubleBuffer<ImageBoxExCom>(), world.getUnitsLayer()));
	}

}*/