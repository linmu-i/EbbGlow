#pragma once

#include <vector>

#include <EbbGlow/Utils/Types.h>

//由于本项目最早基于raylib开发，因此在api设计上大量参考借鉴raylib的设计
//在此感谢raylib项目及其开发者们
//https://www.raylib.com/

// API design inspired by raylib (https://www.raylib.com) — a simple and easy-to-use library for videogames programming.
// Thanks to Ramon Santamaria (@raysan5) and the raylib community.

namespace ebbglow::input
{
	bool InitInput();
	void SwapActiveInputBuffers();

	constexpr uint16_t maxPressure = 1024;

	enum class PointType
	{
		Mouse,
		Touch,
		Pen
	};

	struct InputPoint
	{
		uint32_t id;
		Vec2 position;
		uint16_t pressure;	//0 - 1023
		PointType type;	//Mouse / Touch / Pen
	};

	const std::vector<InputPoint>& PointList();
	bool PointPressed(uint32_t id);
	bool PointReleased(uint32_t id);
	bool PointDown(uint32_t id);
	Vec2 PointDelta(uint32_t id);

	const std::vector<InputPoint>& TouchPointList();

	Vec2 MousePosition();
	Vec2 MouseDelta();
	float MouseWheelDelta();

	enum class Keyboard : uint16_t
    {
		Null = 0,
		Apostrophe = 39,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,
		Zero = 48,
		One = 49,
		Two = 50,
		Three = 51,
		Four = 52,
		Five = 53,
		Six = 54,
		Seven = 55,
		Eight = 56,
		Nine = 57,
		Semicolon = 59,
		Equal = 61,
		A = 65, B = 66, C = 67, D = 68, E = 69,
		F = 70, G = 71, H = 72, I = 73, J = 74,
		K = 75, L = 76, M = 77, N = 78, O = 79,
		P = 80, Q = 81, R = 82, S = 83, T = 84,
		U = 85, V = 86, W = 87, X = 88, Y = 89,
		Z = 90,
		LeftBracket = 91,
		Backslash = 92,
		RightBracket = 93,
		Grave = 96,
		Space = 32,
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290, F2 = 291, F3 = 292, F4 = 293,
		F5 = 294, F6 = 295, F7 = 296, F8 = 297,
		F9 = 298, F10 = 299, F11 = 300, F12 = 301,
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		KbMenu = 348,
		Kp0 = 320, Kp1 = 321, Kp2 = 322, Kp3 = 323,
		Kp4 = 324, Kp5 = 325, Kp6 = 326, Kp7 = 327,
		Kp8 = 328, Kp9 = 329,
		KpDecimal = 330,
		KpDivide = 331,
		KpMultiply = 332,
		KpSubtract = 333,
		KpAdd = 334,
		KpEnter = 335,
		KpEqual = 336,
    };

	bool KeyPressed(Keyboard key);
	bool KeyReleased(Keyboard key);
	bool KeyDown(Keyboard key);
	bool KeyUp(Keyboard key);

	enum class MouseButton : uint8_t
	{
		Left = 0,
		Right = 1,
		Middle = 2,
		Side = 3,
		Extra = 4,
		Forward = 5,
		Back = 6
	};

	bool MousePressed(MouseButton key);
	bool MouseReleased(MouseButton key);
	bool MouseDown(MouseButton key);
	bool MouseUp(MouseButton key);
}