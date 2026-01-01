#pragma once

#include <cmath>
#include <algorithm>

#include <EbbGlow/Utils/Types.h>

namespace ebbglow
{
	[[nodiscard]] inline constexpr Vec2 operator-(const Vec2& a) noexcept
	{
		return Vec2{ -a.x, -a.y};
	}
	[[nodiscard]] inline constexpr Vec2 operator+(const Vec2& a, const Vec2& b) noexcept
	{
		return Vec2{ a.x + b.x, a.y + b.y };
	}
	[[nodiscard]] inline constexpr Vec2 operator-(const Vec2& a, const Vec2& b) noexcept
	{
		return Vec2{ a.x - b.x, a.y - b.y };
	}
	inline constexpr Vec2& operator+=(Vec2& a, const Vec2& b) noexcept
	{
		a.x += b.x;
		a.y += b.y;
		return a;
	}
	inline constexpr Vec2& operator-=(Vec2& a, const Vec2& b) noexcept
	{
		a.x -= b.x;
		a.y -= b.y;
		return a;
	}
	[[nodiscard]] inline constexpr float operator*(const Vec2& a, const Vec2& b) noexcept
	{
		return a.x * b.x + a.y * b.y;
	}
	[[nodiscard]] inline constexpr Vec2 operator*(const Vec2& a, float b) noexcept
	{
		return Vec2{ a.x * b, a.y * b };
	}
	[[nodiscard]] inline constexpr Vec2 operator*(float a, const Vec2& b) noexcept
	{
		return Vec2{ b.x * a, b.y * a };
	}
	[[nodiscard]] inline constexpr Vec2 operator/(const Vec2& a, float b) noexcept
	{
		return Vec2{ a.x / b, a.y / b };
	}
	inline constexpr Vec2& operator*=(Vec2& a, float b) noexcept
	{ 
		a.x *= b;
		a.y *= b;
		return a;
	}
	inline constexpr Vec2& operator/=(Vec2& a, float b) noexcept
	{
		a.x /= b;
		a.y /= b;
		return a;
	}
	[[nodiscard]] inline float Vec2Length(const Vec2& a) noexcept
	{
		return std::sqrt(a.x * a.x + a.y * a.y);
	}
	[[nodiscard]] inline constexpr float Vec2LengthSqr(const Vec2& a) noexcept
	{
		return a.x * a.x + a.y * a.y;
	}
	[[nodiscard]] inline Vec2 Vec2Normalize(const Vec2& a) noexcept
	{
		float length = Vec2Length(a);
		if (length == 0.0f) return Vec2{ 0.0f, 0.0f };
		return a / length;
	}
}