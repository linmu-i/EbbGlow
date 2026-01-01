#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/Utils/Math.h>

#include <vector>
#include <raylib.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define CloseWindow      CloseWindow_WinAPI
#define ShowCursor       ShowCursor_WinAPI

#include <Windows.h>
#include <windowsx.h>

namespace ebbglow::input
{
	static std::vector<InputPoint> points0;
	static std::vector<InputPoint> points1;

	static std::vector<InputPoint> touchPoints0;
	static std::vector<InputPoint> touchPoints1;

	static std::vector<InputPoint> penPoints0;
	static std::vector<InputPoint> penPoints1;

	static InputPoint mousePoint0 = {};
	static InputPoint mousePoint1 = {};

	HWND hwnd = nullptr;
	WNDPROC originalWndProc = nullptr;

	static bool active = false;
	static bool initialized = false;

	static uint32_t mouseId = 0xFFFFFFFF;

	static inline std::vector<InputPoint>& ActPoints() { return active ? points0 : points1; }
	static inline std::vector<InputPoint>& InaPoints() { return active ? points1 : points0; }

	static inline std::vector<InputPoint>& ActTouchPoints() { return active ? touchPoints0 : touchPoints1; }
	static inline std::vector<InputPoint>& InaTouchPoints() { return active ? touchPoints1 : touchPoints0; }

	static inline std::vector<InputPoint>& ActPenPoints() { return active ? penPoints0 : penPoints1; }
	static inline std::vector<InputPoint>& InaPenPoints() { return active ? penPoints1 : penPoints0; }

	static inline InputPoint& ActMousePoint() { return active ? mousePoint0 : mousePoint1; }
	static inline InputPoint& InaMousePoint() { return active ? mousePoint1 : mousePoint0; }

	void SwapActiveInputBuffers()
	{
		active = !active;
		ActPoints() = InaPoints();
		ActTouchPoints() = InaTouchPoints();
		ActPenPoints() = InaPenPoints();
		ActMousePoint() = InaMousePoint();
	}


	static inline auto GetPointerInfoPtr()
	{
		static auto p = reinterpret_cast<decltype(&GetPointerInfo)>(::GetProcAddress(::GetModuleHandleW(L"user32.dll"), "GetPointerInfo"));
		return p;
	}

	static inline auto GetPointerPenInfoPtr()
	{
		static auto p = reinterpret_cast<decltype(&GetPointerPenInfo)>(::GetProcAddress(::GetModuleHandleW(L"user32.dll"), "GetPointerPenInfo"));
		return p;
	}

	InputPoint ReadInfo(const POINTER_INFO& pointerInfo)
	{
		InputPoint point = {};
		point.id = pointerInfo.pointerId;
		POINT pt = pointerInfo.ptPixelLocation;
		ScreenToClient(hwnd, &pt);
		point.position = { static_cast<float>(pt.x), static_cast<float>(pt.y) };
		switch (pointerInfo.pointerType)
		{
		case PT_TOUCH:
			point.type = PointType::Touch;
			point.pressure = maxPressure;
			break;
		case PT_MOUSE:
			point.type = PointType::Mouse;
			point.pressure = 0;
			break;
		case PT_PEN:
			POINTER_PEN_INFO penInfo = {};
			if (!GetPointerPenInfoPtr()(pointerInfo.pointerId, &penInfo)) return {};
			point.type = PointType::Pen;
			point.pressure = penInfo.pressure;
			break;
		}
		return point;
	}

	static LRESULT CALLBACK InputWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		POINTER_INFO pointerInfo = {};
		auto& points = ActPoints();
		auto& touchPoints = ActTouchPoints();
		auto& penPoints = ActPenPoints();
		std::vector<InputPoint>::iterator it;
		switch (msg)
		{
		case WM_POINTERDOWN:
			GetPointerInfoPtr()(GET_POINTERID_WPARAM(wParam), &pointerInfo);
			points.push_back(ReadInfo(pointerInfo));
			switch (pointerInfo.pointerType)
			{
			case PT_TOUCH:
				touchPoints.push_back(points.back());
				break;
			case PT_PEN:
				penPoints.push_back(points.back());
			default:;
			}
			break;

		case WM_POINTERUP:
			GetPointerInfoPtr()(GET_POINTERID_WPARAM(wParam), &pointerInfo);
			it = std::find_if(points.begin(), points.end(), [&pointerInfo](InputPoint& p) { return p.id == pointerInfo.pointerId; });
			if (it != points.end()) points.erase(it);

			it = std::find_if(touchPoints.begin(), touchPoints.end(), [&pointerInfo](InputPoint& p) { return p.id == pointerInfo.pointerId; });
			if (it != touchPoints.end()) touchPoints.erase(it);

			it = std::find_if(penPoints.begin(), penPoints.end(), [&pointerInfo](InputPoint& p) { return p.id == pointerInfo.pointerId; });
			if (it != penPoints.end()) penPoints.erase(it);
			break;

		case WM_POINTERUPDATE:
			GetPointerInfoPtr()(GET_POINTERID_WPARAM(wParam), &pointerInfo);
			it = std::find_if(points.begin(), points.end(), [&pointerInfo](InputPoint& p) { return p.id == pointerInfo.pointerId; });
			if (it != points.end()) *it = ReadInfo(pointerInfo);

			it = std::find_if(touchPoints.begin(), touchPoints.end(), [&pointerInfo](InputPoint& p) { return p.id == pointerInfo.pointerId; });
			if (it != touchPoints.end()) *it = ReadInfo(pointerInfo);

			it = std::find_if(penPoints.begin(), penPoints.end(), [&pointerInfo](InputPoint& p) { return p.id == pointerInfo.pointerId; });
			if (it != penPoints.end()) *it = ReadInfo(pointerInfo);
			break;

		case WM_MOUSEMOVE:
			ActMousePoint().position = {
				static_cast<float>(GET_X_LPARAM(lParam)),
				static_cast<float>(GET_Y_LPARAM(lParam))
			};
			it = std::find_if(points.begin(), points.end(), [](InputPoint& p) { return p.id == mouseId; });
			if (it != points.end()) it->position = ActMousePoint().position;
			break;

		case WM_TOUCH:
		{
			UINT cInputs = LOWORD(wParam);
			std::vector<TOUCHINPUT> inputs(cInputs);
			if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, inputs.data(), sizeof(TOUCHINPUT)))
			{
				for (const auto& ti : inputs)
				{
					InputPoint pt = {};
					pt.id = ti.dwID;
					pt.type = PointType::Touch;
					pt.pressure = maxPressure;

					POINT ptScreen = {
						static_cast<LONG>(ti.x / 100.0f),
						static_cast<LONG>(ti.y / 100.0f)
					};
					ScreenToClient(hwnd, &ptScreen);
					pt.position = {
						static_cast<float>(ptScreen.x),
						static_cast<float>(ptScreen.y)
					};

					if (ti.dwFlags & TOUCHEVENTF_DOWN)
					{
						points.push_back(pt);
						touchPoints.push_back(pt);
					}
					else if (ti.dwFlags & TOUCHEVENTF_UP)
					{
						it = std::find_if(points.begin(), points.end(), [&pt](const InputPoint& p) { return p.id == pt.id; });
						if (it != points.end()) points.erase(it);

						it = std::find_if(touchPoints.begin(), touchPoints.end(), [&pt](const InputPoint& p) { return p.id == pt.id; });
						if (it != touchPoints.end()) touchPoints.erase(it);
					}
					else if (ti.dwFlags & TOUCHEVENTF_MOVE)
					{
						it = std::find_if(points.begin(), points.end(), [&pt](const InputPoint& p) { return p.id == pt.id; });
						if (it != points.end()) *it = pt;

						it = std::find_if(touchPoints.begin(), touchPoints.end(), [&pt](const InputPoint& p) { return p.id == pt.id; });
						if (it != touchPoints.end()) *it = pt;
					}
				}
			}
			CloseTouchInputHandle((HTOUCHINPUT)lParam);
		}
		default:;
		}
		return CallWindowProc(originalWndProc, hwnd, msg, wParam, lParam);
	}
	

	bool InitInput()
	{
		hwnd = (HWND)GetWindowHandle();
		if (!hwnd)
		{
			return false;
		}
		RegisterTouchWindow(hwnd, 0);

		mousePoint0.id = mouseId;
		mousePoint1.id = mouseId;

		points0.push_back(mousePoint0);
		points1.push_back(mousePoint0);

		originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(InputWndProc)));
		return true;
	}

	const std::vector<InputPoint>& PointList() { return ActPoints(); }
	bool PointPressed(uint32_t id)
	{
		auto& actPoints = ActPoints();
		auto act = std::find_if(actPoints.begin(), actPoints.end(), [id](const InputPoint& p) { return p.id == id; });
		
		if (act == actPoints.end()) return false;
		if (act->pressure == 0) return false;

		auto& inaPoints = InaPoints();
		auto ina = std::find_if(inaPoints.begin(), inaPoints.end(), [id](const InputPoint& p) { return p.id == id; });

		if (ina == inaPoints.end()) return true;
		if (ina->pressure == 0) return true;

		return false;
	}
	bool PointReleased(uint32_t id)
	{
		auto& inaPoints = InaPoints();
		auto ina = std::find_if(inaPoints.begin(), inaPoints.end(), [id](const InputPoint& p) { return p.id == id; });

		if (ina == inaPoints.end()) return false;
		if (ina->pressure == 0) return false;

		auto& actPoints = ActPoints();
		auto act = std::find_if(actPoints.begin(), actPoints.end(), [id](const InputPoint& p) { return p.id == id; });

		if (act == actPoints.end()) return true;
		if (act->pressure == 0) return true;

		return false;
	}
	bool PointDown(uint32_t id)
	{
		auto& inaPoints = InaPoints();
		auto ina = std::find_if(inaPoints.begin(), inaPoints.end(), [id](const InputPoint& p) { return p.id == id; });

		if (ina == inaPoints.end()) return false;
		if (ina->pressure == 0) return false;

		auto& actPoints = ActPoints();
		auto act = std::find_if(actPoints.begin(), actPoints.end(), [id](const InputPoint& p) { return p.id == id; });

		if (act == actPoints.end()) return false;
		if (act->pressure == 0) return false;

		return true;
	}
	Vec2 PointDelta(uint32_t id)
	{
		auto& actPoints = ActPoints();
		auto act = std::find_if(actPoints.begin(), actPoints.end(), [id](const InputPoint& p) { return p.id == id; });

		if (act == actPoints.end()) return { 0.0f, 0.0f };

		auto& inaPoints = InaPoints();
		auto ina = std::find_if(inaPoints.begin(), inaPoints.end(), [id](const InputPoint& p) { return p.id == id; });

		if (ina == inaPoints.end()) return { 0.0f, 0.0f };

		return act->position - ina->position;
	}

	const std::vector<InputPoint>& TouchPointList() { return ActTouchPoints(); }

	Vec2 MousePosition()
	{
		::Vector2 pos = GetMousePosition();
		return { pos.x, pos.y };
	}
	Vec2 MouseDelta()
	{
		::Vector2 delta = GetMouseDelta();
		return { delta.x, delta.y };
	}
	float MouseWheelDelta()
	{
		return GetMouseWheelMove();
	}

#include "RLKey.h"
	bool KeyPressed(Keyboard key)
	{
		return IsKeyPressed(RLKey(key));
	}
	bool KeyReleased(Keyboard key)
	{
		return IsKeyReleased(RLKey(key));
	}
	bool KeyDown(Keyboard key)
	{
		return IsKeyDown(RLKey(key));
	}
	bool KeyUp(Keyboard key)
	{
		return IsKeyUp(RLKey(key));
	}

	bool MousePressed(MouseButton mouseButton)
	{
		return IsMouseButtonPressed(RLMouseButton(mouseButton));
	}
	bool MouseReleased(MouseButton mouseButton)
	{
		return IsMouseButtonReleased(RLMouseButton(mouseButton));
	}
	bool MouseDown(MouseButton mouseButton)
	{
		return IsMouseButtonDown(RLMouseButton(mouseButton));
	}
	bool MouseUp(MouseButton mouseButton)
	{
		return IsMouseButtonUp(RLMouseButton(mouseButton));
	}
}