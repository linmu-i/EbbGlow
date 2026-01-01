#pragma once

constexpr static KeyboardKey RLKey(ebbglow::input::Keyboard key) noexcept
{
	switch (static_cast<uint16_t>(key)) {
	case 0:   return KEY_NULL;
	case 39:  return KEY_APOSTROPHE;
	case 44:  return KEY_COMMA;
	case 45:  return KEY_MINUS;
	case 46:  return KEY_PERIOD;
	case 47:  return KEY_SLASH;
	case 48:  return KEY_ZERO;
	case 49:  return KEY_ONE;
	case 50:  return KEY_TWO;
	case 51:  return KEY_THREE;
	case 52:  return KEY_FOUR;
	case 53:  return KEY_FIVE;
	case 54:  return KEY_SIX;
	case 55:  return KEY_SEVEN;
	case 56:  return KEY_EIGHT;
	case 57:  return KEY_NINE;
	case 59:  return KEY_SEMICOLON;
	case 61:  return KEY_EQUAL;
	case 65:  return KEY_A;
	case 66:  return KEY_B;
	case 67:  return KEY_C;
	case 68:  return KEY_D;
	case 69:  return KEY_E;
	case 70:  return KEY_F;
	case 71:  return KEY_G;
	case 72:  return KEY_H;
	case 73:  return KEY_I;
	case 74:  return KEY_J;
	case 75:  return KEY_K;
	case 76:  return KEY_L;
	case 77:  return KEY_M;
	case 78:  return KEY_N;
	case 79:  return KEY_O;
	case 80:  return KEY_P;
	case 81:  return KEY_Q;
	case 82:  return KEY_R;
	case 83:  return KEY_S;
	case 84:  return KEY_T;
	case 85:  return KEY_U;
	case 86:  return KEY_V;
	case 87:  return KEY_W;
	case 88:  return KEY_X;
	case 89:  return KEY_Y;
	case 90:  return KEY_Z;
	case 91:  return KEY_LEFT_BRACKET;
	case 92:  return KEY_BACKSLASH;
	case 93:  return KEY_RIGHT_BRACKET;
	case 96:  return KEY_GRAVE;
	case 32:  return KEY_SPACE;
	case 256: return KEY_ESCAPE;
	case 257: return KEY_ENTER;
	case 258: return KEY_TAB;
	case 259: return KEY_BACKSPACE;
	case 260: return KEY_INSERT;
	case 261: return KEY_DELETE;
	case 262: return KEY_RIGHT;
	case 263: return KEY_LEFT;
	case 264: return KEY_DOWN;
	case 265: return KEY_UP;
	case 266: return KEY_PAGE_UP;
	case 267: return KEY_PAGE_DOWN;
	case 268: return KEY_HOME;
	case 269: return KEY_END;
	case 280: return KEY_CAPS_LOCK;
	case 281: return KEY_SCROLL_LOCK;
	case 282: return KEY_NUM_LOCK;
	case 283: return KEY_PRINT_SCREEN;
	case 284: return KEY_PAUSE;
	case 290: return KEY_F1;
	case 291: return KEY_F2;
	case 292: return KEY_F3;
	case 293: return KEY_F4;
	case 294: return KEY_F5;
	case 295: return KEY_F6;
	case 296: return KEY_F7;
	case 297: return KEY_F8;
	case 298: return KEY_F9;
	case 299: return KEY_F10;
	case 300: return KEY_F11;
	case 301: return KEY_F12;
	case 340: return KEY_LEFT_SHIFT;
	case 341: return KEY_LEFT_CONTROL;
	case 342: return KEY_LEFT_ALT;
	case 343: return KEY_LEFT_SUPER;
	case 344: return KEY_RIGHT_SHIFT;
	case 345: return KEY_RIGHT_CONTROL;
	case 346: return KEY_RIGHT_ALT;
	case 347: return KEY_RIGHT_SUPER;
	case 348: return KEY_KB_MENU;
	case 320: return KEY_KP_0;
	case 321: return KEY_KP_1;
	case 322: return KEY_KP_2;
	case 323: return KEY_KP_3;
	case 324: return KEY_KP_4;
	case 325: return KEY_KP_5;
	case 326: return KEY_KP_6;
	case 327: return KEY_KP_7;
	case 328: return KEY_KP_8;
	case 329: return KEY_KP_9;
	case 330: return KEY_KP_DECIMAL;
	case 331: return KEY_KP_DIVIDE;
	case 332: return KEY_KP_MULTIPLY;
	case 333: return KEY_KP_SUBTRACT;
	case 334: return KEY_KP_ADD;
	case 335: return KEY_KP_ENTER;
	case 336: return KEY_KP_EQUAL;

	default:  return KEY_NULL;
	}
}

constexpr static ::MouseButton RLMouseButton(ebbglow::input::MouseButton mouseButton) noexcept
{
	switch (static_cast<int>(mouseButton))
	{
	case 0: return MOUSE_BUTTON_LEFT;
	case 1: return MOUSE_BUTTON_RIGHT;
	case 2: return MOUSE_BUTTON_MIDDLE;
	case 3: return MOUSE_BUTTON_SIDE;
	case 4: return MOUSE_BUTTON_EXTRA;
	case 5: return MOUSE_BUTTON_FORWARD;
	default: return MOUSE_BUTTON_BACK;
	}
}