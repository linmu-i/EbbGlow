#pragma once

#include <raylib.h>
#include <EbbGlow/Utils/Types.h>

inline constexpr ::Color RLColor(const ebbglow::ColorR8G8B8A8& color) noexcept
{
	return ::Color{ color.r, color.g, color.b, color.a };
}

inline ::Color RLColor(const ebbglow::ColorHSV& color) noexcept
{
	return ColorFromHSV(color.h, color.s, color.v);;
}

inline constexpr ::Vector2 RLVec2(const ebbglow::Vec2& vec2) noexcept
{
	return ::Vector2{ vec2.x, vec2.y };
}

inline constexpr ::Rectangle RLRect(const ebbglow::Rect& rect) noexcept
{
	return ::Rectangle{ rect.x, rect.y, rect.width, rect.height };
}

inline constexpr ::Camera2D RLCamera2D(const ebbglow::Camera2D& camera2D)
{
	return { RLVec2(camera2D.offset), RLVec2(camera2D.target), camera2D.rotation, camera2D.zoom };
}