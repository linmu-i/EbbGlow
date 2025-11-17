#pragma once
#include <raylib.h>

#ifdef __cplusplus
extern "C" {
#endif

	// 初始化触摸支持
	bool InitTouchEx();

	// 获取当前触摸点数量
	int GetTouchPointCountEx();

	// 获取指定索引的触摸点坐标
	Vector2 GetTouchPositionEx(int index);

	// 获取触摸点压力（如果支持）
	float GetTouchPressureEx(int index);

	// 检查是否支持触摸
	bool IsTouchExSupported();

	// 清理触摸资源
	void CloseTouchEx();

#ifdef __cplusplus
}
#endif

#ifndef _WIN32
static inline int GetTouchCount()
{
	return GetTouchPointCount();
}

static inline Vector2 GetTouchPoint(int index)
{
	return GetTouchPosition(index);
}

static inline float GetTouchPressure(int index)
{
	// Raylib does not provide pressure information; return a default value
	return 1.0f;
}

static inline void CloseTouch()
{
	// No specific cleanup required for touch in Raylib
}

static inline bool InitTouch()
{
	// No specific initialization required for touch in Raylib
	return true;
}
static inline bool IsTouchSupported()
{
	return true; //no uesd
}
#endif //_WIN32