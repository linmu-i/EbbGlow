#include <raylib.h>
#include <cmath>
#include <EbbGlow/Graphics/Graphics.h>
#include "../Utils/RLTypesCast.h"
#include "../Utils/ResourceCreator.h"

namespace ebbglow::gfx
{
	void DrawPixel(Vec2 pos, Color color)
	{
		DrawPixelV({ pos.x, pos.y }, RLColor(color));
	}

	void DrawLine(Vec2 startPos, Vec2 endPos, Color color, float lineThick)
	{
		DrawLineEx(RLVec2(startPos), RLVec2(endPos), lineThick, RLColor(color));
	}

	void DrawRect(Rect rect, Color color, float rotation, Vec2 origin)
	{
		if (rotation == 0.0f)
		{
			DrawRectangleRec(RLRect(rect), RLColor(color));
		}
		else
		{
			DrawRectanglePro(RLRect(rect), RLVec2(origin), rotation, RLColor(color));
		}
	}

	void DrawRectLines(Rect rect, Color color, float lineThick)
	{
		DrawRectangleLinesEx(RLRect(rect), lineThick, RLColor(color));
	}

	void DrawRectangleGradientV(Rect rect, Color color1, Color color2)
	{
		DrawRectangleGradientV(
			std::roundl(rect.x),
			std::roundl(rect.y),
			std::roundl(rect.width),
			std::roundl(rect.height),
			RLColor(color1),
			RLColor(color2)
		);
	}

	void DrawRectangleGradientH(Rect rect, Color color1, Color color2)
	{
		DrawRectangleGradientH(
			std::roundl(rect.x),
			std::roundl(rect.y),
			std::roundl(rect.width),
			std::roundl(rect.height),
			RLColor(color1),
			RLColor(color2)
		);
	}

	void DrawCircle(Vec2 center, float radius, Color color)
	{
		DrawCircleV(RLVec2(center), radius, RLColor(color));
	}

	void DrawCircleLines(Vec2 center, float radius, Color color, float lineThick)
	{
		if (lineThick <= 1.0f)
		{
			DrawCircleLines(std::roundl(center.x), std::roundl(center.y), radius, RLColor(color));
		}
		else
		{
			for (float r = radius - lineThick / 2.0f; r <= radius + lineThick / 2.0f; r += 1.0f)
			{
				DrawCircleLines(std::roundl(center.x), std::roundl(center.y), r, RLColor(color));
			}
		}
	}

	void DrawCircleGradient(Vec2 pos, float radius, Color color1, Color color2)
	{
		DrawCircleGradient(std::roundl(pos.x), std::roundl(pos.y), radius, RLColor(color1), RLColor(color2));
	}

	void DrawCircleSector(Vec2 center, float radius, float startAngle, float endAngle, Color color, int segments)
	{
		DrawCircleSector(RLVec2(center), radius, startAngle, endAngle, segments, RLColor(color));
	}

	void DrawCircleSectorLines(Vec2 center, float radius, float startAngle, float endAngle, Color color, float lineThick, int segments)
	{
		float halfLineThick = lineThick / 2.0f;
		DrawRing(center, radius - halfLineThick, radius + halfLineThick, startAngle, endAngle, color);
	}

	void DrawRing(Vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, Color color, int segments)
	{
		DrawRing(RLVec2(center), innerRadius, outerRadius, startAngle, endAngle, segments, RLColor(color));
	}

	void DrawRingLines(Vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, Color color, float lineThick, int segments)
	{
		float halfLineThick = lineThick / 2.0f;
		DrawRing(center, innerRadius - halfLineThick, innerRadius + halfLineThick, startAngle, endAngle, color);
		DrawRing(center, outerRadius - halfLineThick, outerRadius + halfLineThick, startAngle, endAngle, color);
	}

	void DrawTriangle(Vec2 v1, Vec2 v2, Vec2 v3, Color color)
	{
		DrawTriangle(RLVec2(v1), RLVec2(v2), RLVec2(v3), RLColor(color));
	}

	void DrawTriangleLines(Vec2 v1, Vec2 v2, Vec2 v3, Color color)
	{
		DrawTriangleLines(RLVec2(v1), RLVec2(v2), RLVec2(v3), RLColor(color));
	}

	void DrawTriangleFan(const std::vector<Vec2>& pos, Color color)
	{
		std::vector<Vector2> rlPoints;
		rlPoints.reserve(pos.size());
		for (const Vec2& p : pos)
		{
			rlPoints.push_back(RLVec2(p));
		}
		DrawTriangleFan(rlPoints.data(), static_cast<int>(rlPoints.size()), RLColor(color));
	}

	void DrawTriangleStrip(const std::vector<Vec2>& pos, Color color)
	{
		std::vector<Vector2> rlPoints;
		rlPoints.reserve(pos.size());
		for (const Vec2& p : pos)
		{
			rlPoints.push_back(RLVec2(p));
		}
		DrawTriangleStrip(rlPoints.data(), static_cast<int>(rlPoints.size()), RLColor(color));
	}

	void DrawPoly(Vec2 center, int sides, float radius, float rotation, Color color)
	{
		DrawPoly(RLVec2(center), sides, radius, rotation, RLColor(color));
	}

	void DrawPolyLines(Vec2 center, int sides, float radius, float rotation, Color color, float lineThick)
	{
		DrawPolyLinesEx(RLVec2(center), sides, radius, rotation, lineThick, RLColor(color));
	}

	void DrawTexture(const rsc::SharedTexture2D& texture, Vec2 pos, float scale, float rotation, Color tint)
	{
		DrawTextureEx(*static_cast<Texture2D*>(texture.get()), RLVec2(pos), rotation, scale, RLColor(tint));
	}

	void DrawTexturePro(const rsc::SharedTexture2D& texture, Rect sourceRec, Rect destRec, Vec2 origin, float rotation, Color tint)
	{
		DrawTexturePro(*static_cast<Texture2D*>(texture.get()), RLRect(sourceRec), RLRect(destRec), RLVec2(origin), rotation, RLColor(tint));
	}

	void DrawTextureRegion(const rsc::SharedTexture2D& texture, Rect sourceRec, Vec2 pos, Color tint)
	{
		DrawTextureRec(*static_cast<Texture2D*>(texture.get()), RLRect(sourceRec), RLVec2(pos), RLColor(tint));
	}

	void DrawTexture(const rsc::SharedRenderTexture2D& texture, Vec2 pos, float scale, float rotation, Color tint)
	{
		DrawTextureEx(static_cast<RenderTexture2D*>(texture.get())->texture, RLVec2(pos), rotation, scale, RLColor(tint));
	}

	void DrawTexturePro(const rsc::SharedRenderTexture2D& texture, Rect sourceRec, Rect destRec, Vec2 origin, float rotation, Color tint)
	{
		DrawTexturePro(static_cast<RenderTexture2D*>(texture.get())->texture, RLRect(sourceRec), RLRect(destRec), RLVec2(origin), rotation, RLColor(tint));
	}

	void DrawTextureRegion(const rsc::SharedRenderTexture2D& texture, Rect sourceRec, Vec2 pos, Color tint)
	{
		DrawTextureRec(static_cast<RenderTexture2D*>(texture.get())->texture, RLRect(sourceRec), RLVec2(pos), RLColor(tint));
	}

	void DrawText(const rsc::SharedFont& font, const std::string& text, Vec2 position, float fontSize, float spacing, Color tint, Vec2 origin, float rotation)
	{
		DrawTextPro(*static_cast<Font*>(font.get()), text.c_str(), RLVec2(position), RLVec2(origin), rotation, fontSize, spacing, RLColor(tint));
	}

	void DrawTextCodepoints(const rsc::SharedFont& font, const std::vector<int32_t>& codepoints, Vec2 position, float fontSize, float spacing, Color tint)
	{
		DrawTextCodepoints(*static_cast<Font*>(font.get()), codepoints.data(), static_cast<int>(codepoints.size()), RLVec2(position), fontSize, spacing, RLColor(tint));
	}

	const char* DefaultSDFShaderCode = R"(
#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;

void main()
{
	vec2 params = vec2(0.5, 0.05);
	float d = texture(texture0, fragTexCoord).a;
	float a = smoothstep(params.x - params.y, params.x + params.y, d);
	finalColor = vec4(fragColor.rgb, fragColor.a * a);
}
)";
	const char* DefaultAAShaderCode = R"(
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;

void main()
{
	vec4 center = texture(texture0, fragTexCoord);
	vec2 delta = 1.0 / vec2(textureSize(texture0, 0));
	float alpha = 0.0;
	for (int w = -1; w <= 1; ++w)
	{
		for (int h = -1; h <= 1; ++h)
		{
			if (w == 0 && h == 0)
			{
				alpha += center.a * 0.6;
			}
			else if (abs(w) == abs(h))
			{
				alpha += texture(texture0, fragTexCoord + vec2(w, h) * delta).a * 0.15 / 4;
			}
			else
			{
				alpha += texture(texture0, fragTexCoord + vec2(w, h) * delta).a * 0.0625;
			}
		}
	}
	alpha = pow(alpha, 0.35);
	if (alpha > 0.95) alpha = 1;
	finalColor = vec4(1, 1, 1, alpha) * fragColor;
}
)";
	
	rsc::SharedShader& GetDefaultSDFShader()
	{
		static rsc::ResourceCreator creator;
		static rsc::SharedShader defaultShader = creator.CreateShader(LoadShaderFromMemory(nullptr, DefaultSDFShaderCode));
		return defaultShader;
	}

	rsc::SharedShader& GetDefaultAAShader()
	{
		static rsc::ResourceCreator creator;
		static rsc::SharedShader defaultShader = creator.CreateShader(LoadShaderFromMemory(nullptr, DefaultAAShaderCode));
		return defaultShader;
	}

	void DrawSDFText(const rsc::SharedFont& font, const std::string& text, Vec2 position, float fontSize, float spacing, Color tint, Vec2 origin, float rotation, const rsc::SharedShader& shader)
	{
		BeginShaderMode(*static_cast<Shader*>(shader.get()));
		DrawTextPro(*static_cast<Font*>(font.get()), text.c_str(), RLVec2(position), RLVec2(origin), rotation, fontSize, spacing, RLColor(tint));
		EndShaderMode();
	}

	void DrawSDFTextCodepoints(const rsc::SharedFont& font, const std::vector<int32_t>& codepoints, Vec2 position, float fontSize, float spacing, Color tint, const rsc::SharedShader& shader)
	{
		BeginShaderMode(*static_cast<Shader*>(shader.get()));
		DrawTextCodepoints(*static_cast<Font*>(font.get()), codepoints.data(), static_cast<int>(codepoints.size()), RLVec2(position), fontSize, spacing, RLColor(tint));
		EndShaderMode();
	}

	void DrawLineStrip(const std::vector<Vec2>& points, Color color, float lineThick)
	{
		std::vector<Vector2> rlPoints;
		rlPoints.reserve(points.size());
		for (const Vec2& p : points)
		{
			rlPoints.push_back(RLVec2(p));
		}
		DrawSplineLinear(rlPoints.data(), static_cast<int>(rlPoints.size()), lineThick, RLColor(color));
	}

	void ClearBackground(Color color)
	{
		ClearBackground(RLColor(color));
	}
}