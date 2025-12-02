#include <platform/Platform.h>

#ifdef _WIN32

#define CloseWindow    Win32CloseWindow
#define ShowCursor     Win32ShowCursor
#define Rectangle      Win32RECT
#include <map>
#include <vector>
#include <algorithm>
#include <Windows.h>

// 避免与 Windows API 冲突（Raylib 自定义类型）


struct TouchPoint {
    uint32_t id;
    Vector2 position;
    bool isActive;
    float pressure;
};

static std::map<uint32_t, TouchPoint> g_touchPoints;
static WNDPROC g_originalWndProc = nullptr;
static HWND g_windowHandle = nullptr;
static bool g_touchInitialized = false;

static bool GetTouchDataFromPointer(UINT32 pointerId, TouchPoint& outPoint) {
    POINTER_INFO pi;
    if (!GetPointerInfo(pointerId, &pi)) {
        return false;
    }

    // 仅处理触摸和触控笔，跳过鼠标输入
    if (pi.pointerType == PT_MOUSE) {
        return false;
    }

    POINT pt = { static_cast<LONG>(pi.ptPixelLocation.x), static_cast<LONG>(pi.ptPixelLocation.y) };
    if (!ScreenToClient(g_windowHandle, &pt)) {
        return false;
    }

    outPoint.id = static_cast<uint32_t>(pointerId);
    outPoint.position = { static_cast<float>(pt.x), static_cast<float>(pt.y) };
    outPoint.isActive = true;

    float pressure = 1.0f;

    if (pi.pointerType == PT_PEN) {
        POINTER_PEN_INFO ppi;
        if (GetPointerPenInfo(pointerId, &ppi)) {
            if (ppi.pressure > 0) {
                // 使用通用默认最大压力值 1024（兼容旧 SDK，避免 pressureMax 不存在）
                const float MAX_PRESSURE = 1024.0f;
                pressure = static_cast<float>(ppi.pressure) / MAX_PRESSURE;
                if (pressure > 1.0f) pressure = 1.0f;
            }
        }
    }

    outPoint.pressure = pressure;
    return true;
}

static LRESULT CALLBACK TouchWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_POINTERDOWN:
    case WM_POINTERUPDATE:
    case WM_POINTERUP:
    {
        UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
        TouchPoint tp;

        if (GetTouchDataFromPointer(pointerId, tp)) {
            if (msg == WM_POINTERDOWN) {
                g_touchPoints[pointerId] = tp;
            }
            else if (msg == WM_POINTERUPDATE) {
                auto it = g_touchPoints.find(pointerId);
                if (it != g_touchPoints.end()) {
                    it->second.position = tp.position;
                    it->second.pressure = tp.pressure;
                }
            }
            else if (msg == WM_POINTERUP) {
                g_touchPoints.erase(pointerId);
            }
        }
        return 0;
    }

    case WM_TOUCH:
    {
        HTOUCHINPUT hTouchInput = (HTOUCHINPUT)lParam;
		UINT touchCount = LOWORD(wParam);
		std::vector<TOUCHINPUT> touchInputs(touchCount);
        GetTouchInputInfo(hTouchInput, touchCount, touchInputs.data(), sizeof(TOUCHINPUT));
        for (UINT i = 0; i < touchCount; ++i) {
            TOUCHINPUT& ti = touchInputs[i];
            uint32_t id = static_cast<uint32_t>(ti.dwID);
            POINT pt = { ti.x / 100, ti.y / 100 };
            ScreenToClient(hWnd, &pt);
            if (ti.dwFlags & TOUCHEVENTF_DOWN) {
                TouchPoint tp;
                tp.id = id;
                tp.position = { static_cast<float>(pt.x), static_cast<float>(pt.y) };
                tp.isActive = true;
                tp.pressure = 1.0f; // Windows Touch API 不提供压力信息
                g_touchPoints[id] = tp;
            }
            else if (ti.dwFlags & TOUCHEVENTF_MOVE) {
                auto it = g_touchPoints.find(id);
                if (it != g_touchPoints.end()) {
                    it->second.position = { static_cast<float>(pt.x), static_cast<float>(pt.y) };
                }
            }
            else if (ti.dwFlags & TOUCHEVENTF_UP) {
                g_touchPoints.erase(id);
            }
		}
		CloseTouchInputHandle(hTouchInput);
        return 0;
    }

    case WM_DESTROY:
        if (g_touchInitialized) {
            g_touchInitialized = false;
        }
        break;
    }

    return CallWindowProc(g_originalWndProc, hWnd, msg, wParam, lParam);
}

bool InitTouchEx() {
    if (g_touchInitialized) {
        return true;
    }

    g_windowHandle = GetActiveWindow();
    if (!g_windowHandle) {
        return false;
    }

    RegisterTouchWindow(g_windowHandle, 0);
    g_originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(g_windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TouchWindowProc))
        );
    if (!g_originalWndProc) {
        return false;
    }

    g_touchInitialized = true;
    return true;
}

int GetTouchPointCountEx() {
    return static_cast<int>(g_touchPoints.size());
}

Vector2 GetTouchPositionEx(int index) {
    if (index < 0 || index >= static_cast<int>(g_touchPoints.size())) {
        return { 0.0f, 0.0f };
    }

    int currentIndex = 0;
    for (const auto& pair : g_touchPoints) {
        if (currentIndex == index) {
            return pair.second.position;
        }
        ++currentIndex;
    }

    return { 0.0f, 0.0f };
}

float GetTouchPressureEx(int index) {
    if (index < 0 || index >= static_cast<int>(g_touchPoints.size())) {
        return 0.0f;
    }

    int currentIndex = 0;
    for (const auto& pair : g_touchPoints) {
        if (currentIndex == index) {
            return pair.second.pressure;
        }
        ++currentIndex;
    }

    return 0.0f;
}

bool IsTouchExSupported() {
    return g_touchInitialized;
}

void CloseTouchEx() {
    if (g_touchInitialized && g_windowHandle && g_originalWndProc) {
        SetWindowLongPtr(g_windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_originalWndProc));
        g_touchInitialized = false;
        g_touchPoints.clear();
        UnregisterTouchWindow(g_windowHandle);
    }
}

char* LoadANSI(const char* utf8, int length)
{
    int utf16Length = MultiByteToWideChar(CP_UTF8, 0, utf8, length, nullptr, 0);

    wchar_t* utf16 = (wchar_t*)malloc(utf16Length * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8, length, utf16, utf16Length);

    int ansiLength = WideCharToMultiByte(CP_ACP, 0, utf16, utf16Length, nullptr, 0, nullptr, nullptr);
    char* ansi = (char*)malloc(ansiLength + 1);
    ansi[ansiLength * sizeof(char)] = '\0';
    WideCharToMultiByte(CP_ACP, 0, utf16, utf16Length, ansi, ansiLength, nullptr, nullptr);
    free(utf16);
    return ansi;
}

void UnloadANSI(char* ansi)
{
    free(ansi);
}


#endif // _WIN32