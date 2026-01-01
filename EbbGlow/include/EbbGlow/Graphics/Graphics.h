#pragma once

#include <vector>
#include <string>

#include <EbbGlow/Utils/Resource.h>
#include <EbbGlow/Utils/Types.h>

namespace ebbglow::gfx
{
	void DrawPixel(Vec2 pos, Color color);

	void DrawLine(Vec2 startPos, Vec2 endPos, Color color, float lineThick = 1.0f);


	void DrawRect(Rect rect, Color color, float rotation = 0.0f, Vec2 origin = { 0.0f, 0.0f });
	void DrawRectLines(Rect rect, Color color, float lineThick = 1.0f);
	void DrawRectangleGradientV(Rect rect, Color color1, Color color2);
	void DrawRectangleGradientH(Rect rect, Color color1, Color color2);

	void DrawCircle(Vec2 center, float radius, Color color);
	void DrawCircleLines(Vec2 center, float radius, Color color, float lineThick = 1.0f);
	void DrawCircleGradient(Vec2 pos, float radius, Color color1, Color color2);

	void DrawCircleSector(Vec2 center, float radius, float startAngle, float endAngle, Color color, int segments = 32);
	void DrawCircleSectorLines(Vec2 center, float radius, float startAngle, float endAngle, Color color, float lineThick = 1.0f, int segments = 32);

	void DrawRing(Vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, Color color, int segments = 32);
	void DrawRingLines(Vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, Color color, float lineThick = 1.0f, int segments = 32);

	void DrawTriangle(Vec2 v1, Vec2 v2, Vec2 v3, Color color);
	void DrawTriangleLines(Vec2 v1, Vec2 v2, Vec2 v3, Color color);

	void DrawTriangleFan(const std::vector<Vec2>& pos, Color color);
	void DrawTriangleStrip(const std::vector<Vec2>& pos, Color color);

	void DrawPoly(Vec2 center, int sides, float radius, float rotation, Color color);
	void DrawPolyLines(Vec2 center, int sides, float radius, float rotation, Color color, float lineThick = 1.0f);

	void DrawTexture(const rsc::SharedTexture2D& texture, Vec2 pos, float scale = 1.0f, float rotation = 0.0f, Color tint = { 255, 255, 255, 255 });
	void DrawTexturePro(const rsc::SharedTexture2D& texture, Rect sourceRec, Rect destRec, Vec2 origin = { 0.0f, 0.0f }, float rotation = 0.0f, Color tint = { 255, 255, 255, 255 });
	void DrawTextureRegion(const rsc::SharedTexture2D& texture, Rect sourceRec, Vec2 pos, Color tint = { 255, 255, 255, 255 });

	void DrawTexture(const rsc::SharedRenderTexture2D& texture, Vec2 pos, float scale = 1.0f, float rotation = 0.0f, Color tint = { 255, 255, 255, 255 });
	void DrawTexturePro(const rsc::SharedRenderTexture2D& texture, Rect sourceRec, Rect destRec, Vec2 origin = { 0.0f, 0.0f }, float rotation = 0.0f, Color tint = { 255, 255, 255, 255 });
	void DrawTextureRegion(const rsc::SharedRenderTexture2D& texture, Rect sourceRec, Vec2 pos, Color tint = { 255, 255, 255, 255 });

	void DrawText(const rsc::SharedFont& font, const std::string& text, Vec2 position, float fontSize, float spacing, Color tint = colors::White, Vec2 origin = { 0.0f, 0.0f }, float rotation = 0.0f);
	void DrawTextCodepoints(const rsc::SharedFont& font, const std::vector<int32_t>& codepoints, Vec2 position, float fontSize, float spacing, Color tint = colors::White);

	rsc::SharedShader& GetDefaultSDFShader();
	void DrawSDFText(const rsc::SharedFont& font, const std::string& text, Vec2 position, float fontSize, float spacing, Color tint = colors::White, Vec2 origin = { 0.0f, 0.0f }, float rotation = 0.0f, const rsc::SharedShader& shader = GetDefaultSDFShader());
	void DrawSDFTextCodepoints(const rsc::SharedFont& font, const std::vector<int32_t>& codepoints, Vec2 position, float fontSize, float spacing, Color tint = colors::White, const rsc::SharedShader& shader = GetDefaultSDFShader());

	void DrawLineStrip(const std::vector<Vec2>& points, Color color, float lineThick = 1.0f);

	void ClearBackground(Color color);

	rsc::SharedShader& GetDefaultAAShader();
}