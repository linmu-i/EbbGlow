#include <raylib.h>
#include <cmath>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Math.h>
#include "../Utils/RLTypesCast.h"
#include "../Resources/ResourceCreator.h"

namespace ebbglow::gfx
{
	void DrawPixel(Vec2 pos, Color color)
	{
		::DrawPixelV({ pos.x, pos.y }, RLColor(color));
	}

	void DrawLine(Vec2 startPos, Vec2 endPos, Color color, float lineThick)
	{
		::DrawLineEx(RLVec2(startPos), RLVec2(endPos), lineThick, RLColor(color));
	}

	void DrawRect(Rect rect, Color color, float scale, float rotation, Vec2 pivot)
	{
		if (rotation == 0.0f)
		{
			::DrawRectangleRec(RLRect(rect), RLColor(color));
		}
		else
		{
			pivot += rect.position();
			Rect finalRect = rect.scaleAround(pivot, scale);
			Vec2 lt = finalRect.position().rotatedAround(pivot, rotation);
			Vec2 rt = Vec2{ finalRect.x + finalRect.width, finalRect.y }.rotatedAround(pivot, rotation);
			Vec2 rb = Vec2{ finalRect.x + finalRect.width, finalRect.y + finalRect.height }.rotatedAround(pivot, rotation);
			Vec2 lb = Vec2{ finalRect.x, finalRect.y + finalRect.height }.rotatedAround(pivot, rotation);
			DrawTriangle(rt, lt, lb, color);
			DrawTriangle(lb, rb, rt, color);
		}
	}

	void DrawRectLines(Rect rect, Color color, float lineThick, float scale, float rotation, Vec2 pivot)
	{
		if (rotation == 0.0f) ::DrawRectangleLinesEx(RLRect(rect.scaleAround(pivot, scale)), lineThick, RLColor(color));
		else
		{
			pivot += rect.position();
			auto scaledRect = rect.scaleAround(pivot, scale);
			Vec2 lt = scaledRect.position().rotatedAround(pivot, rotation);
			Vec2 rt = Vec2{ scaledRect.x + scaledRect.width, scaledRect.y }.rotatedAround(pivot, rotation);
			Vec2 rb = Vec2{ scaledRect.x + scaledRect.width, scaledRect.y + scaledRect.height }.rotatedAround(pivot, rotation);
			Vec2 lb = Vec2{ scaledRect.x, scaledRect.y + scaledRect.height }.rotatedAround(pivot, rotation);

			Vec2 off = (lt - rt).normalized() * (lineThick / 2.0f);

			DrawLine(lt + off, rt - off, color, lineThick);
			DrawLine(rt, rb, color, lineThick);
			DrawLine(rb - off, lb + off, color, lineThick);
			DrawLine(lb, lt, color, lineThick);
		}
	}

	void DrawRectangleGradientV(Rect rect, Color color1, Color color2)
	{
		::DrawRectangleGradientV(
			static_cast<int>(lroundf(rect.x)),
			static_cast<int>(lroundf(rect.y)),
			static_cast<int>(lroundf(rect.width)),
			static_cast<int>(lroundf(rect.height)),
			RLColor(color1),
			RLColor(color2)
		);
	}

	void DrawRectangleGradientH(Rect rect, Color color1, Color color2)
	{
		::DrawRectangleGradientH(
			static_cast<int>(lroundf(rect.x)),
			static_cast<int>(lroundf(rect.y)),
			static_cast<int>(lroundf(rect.width)),
			static_cast<int>(lroundf(rect.height)),
			RLColor(color1),
			RLColor(color2)
		);
	}

	void DrawRectRounded(Rect rect, float roundness, Color color, float scale, float rotation, Vec2 pivot, int segments)
	{
		if (rotation == 0.0f)
		{
			::DrawRectangleRounded(RLRect(rect.scaleAround((pivot + rect.position()), scale)), roundness, segments, RLColor(color));
		}
		else
		{
			Vec2 worldOrigin = pivot + rect.position();
			float radius = std::min(rect.width, rect.height) * roundness * scale;
			Rect scaledRect = rect.scaleAround(worldOrigin, scale);
			Vec2 lt = Vec2{ scaledRect.x, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 rt = Vec2{ scaledRect.x + scaledRect.width, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 rb = Vec2{ scaledRect.x + scaledRect.width, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);
			Vec2 lb = Vec2{ scaledRect.x, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);
			Vec2 tl = Vec2{ scaledRect.x + radius, scaledRect.y }.rotatedAround(worldOrigin, rotation);
			Vec2 tr = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y }.rotatedAround(worldOrigin, rotation);
			Vec2 br = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y + scaledRect.height }.rotatedAround(worldOrigin, rotation);
			Vec2 bl = Vec2{ scaledRect.x + radius, scaledRect.y + scaledRect.height }.rotatedAround(worldOrigin, rotation);
			Vec2 clt = Vec2{ scaledRect.x + radius, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 crt = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 crb = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);
			Vec2 clb = Vec2{ scaledRect.x + radius, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);

			DrawTriangle(tl, clt, crt, color);
			DrawTriangle(crt, tr, tl, color);

			DrawTriangle(clb, clt, lt, color);
			DrawTriangle(lt, lb, clb, color);

			DrawTriangle(clb, bl, br, color);
			DrawTriangle(br, crb, clb, color);

			DrawTriangle(crt, crb, rb, color);
			DrawTriangle(rb, rt, crt, color);

			DrawTriangle(crt, clt, clb, color);
			DrawTriangle(clb, crb, crt, color);

			float rotCltBegin = (tl - clt).rad();
			float rotCltEnd = (lt - clt).rad();
			rotCltEnd = rotCltEnd < rotCltBegin ? rotCltEnd : rotCltEnd - 2 * std::numbers::pi_v<float>;

			float rotCrtBegin = (rt - crt).rad();
			float rotCrtEnd = (tr - crt).rad();
			rotCrtEnd = rotCrtEnd < rotCrtBegin ? rotCrtEnd : rotCrtEnd - 2 * std::numbers::pi_v<float>;

			float rotCrbBegin = (br - crb).rad();
			float rotCrbEnd = (rb - crb).rad();
			rotCrbEnd = rotCrbEnd < rotCrbBegin ? rotCrbEnd : rotCrbEnd - 2 * std::numbers::pi_v<float>;

			float rotClbBegin = (lb - clb).rad();
			float rotClbEnd = (bl - clb).rad();
			rotClbEnd = rotClbEnd < rotClbBegin ? rotClbEnd : rotClbEnd - 2 * std::numbers::pi_v<float>;

			DrawCircleSector(clt, radius, rotCltBegin, rotCltEnd, color, segments);
			DrawCircleSector(crt, radius, rotCrtBegin, rotCrtEnd, color, segments);
			DrawCircleSector(crb, radius, rotCrbBegin, rotCrbEnd, color, segments);
			DrawCircleSector(clb, radius, rotClbBegin, rotClbEnd, color, segments);
		}
	}
	void DrawRectRoundedLines(Rect rect, float roundness, Color color, float lineThick, float scale, float rotation, Vec2 pivot, int segments)
	{
		if (rotation == 0.0f)
		{
			::DrawRectangleRoundedLinesEx(RLRect(rect.scaleAround((pivot + rect.position()), scale)), roundness, segments, lineThick, RLColor(color));
		}
		else
		{
			Vec2 worldOrigin = pivot + rect.position();
			float radius = std::min(rect.width, rect.height) * roundness * scale;
			Rect scaledRect = rect.scaleAround(worldOrigin, scale);
			Vec2 lt = Vec2{ scaledRect.x, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 rt = Vec2{ scaledRect.x + scaledRect.width, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 rb = Vec2{ scaledRect.x + scaledRect.width, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);
			Vec2 lb = Vec2{ scaledRect.x, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);
			Vec2 tl = Vec2{ scaledRect.x + radius, scaledRect.y }.rotatedAround(worldOrigin, rotation);
			Vec2 tr = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y }.rotatedAround(worldOrigin, rotation);
			Vec2 br = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y + scaledRect.height }.rotatedAround(worldOrigin, rotation);
			Vec2 bl = Vec2{ scaledRect.x + radius, scaledRect.y + scaledRect.height }.rotatedAround(worldOrigin, rotation);
			Vec2 clt = Vec2{ scaledRect.x + radius, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 crt = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y + radius }.rotatedAround(worldOrigin, rotation);
			Vec2 crb = Vec2{ scaledRect.x + scaledRect.width - radius, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);
			Vec2 clb = Vec2{ scaledRect.x + radius, scaledRect.y + scaledRect.height - radius }.rotatedAround(worldOrigin, rotation);

			DrawLine(lt, lb, color, lineThick);
			DrawLine(rt, rb, color, lineThick);
			DrawLine(tl, tr, color, lineThick);
			DrawLine(bl, br, color, lineThick);

			float rotCltBegin = (tl - clt).rad();
			float rotCltEnd = (lt - clt).rad();
			rotCltEnd = rotCltEnd < rotCltBegin ? rotCltEnd : rotCltEnd - 2 * std::numbers::pi_v<float>;

			float rotCrtBegin = (rt - crt).rad();
			float rotCrtEnd = (tr - crt).rad();
			rotCrtEnd = rotCrtEnd < rotCrtBegin ? rotCrtEnd : rotCrtEnd - 2 * std::numbers::pi_v<float>;

			float rotCrbBegin = (br - crb).rad();
			float rotCrbEnd = (rb - crb).rad();
			rotCrbEnd = rotCrbEnd < rotCrbBegin ? rotCrbEnd : rotCrbEnd - 2 * std::numbers::pi_v<float>;

			float rotClbBegin = (lb - clb).rad();
			float rotClbEnd = (bl - clb).rad();
			rotClbEnd = rotClbEnd < rotClbBegin ? rotClbEnd : rotClbEnd - 2 * std::numbers::pi_v<float>;

			DrawCircleSectorLines(clt, radius, rotCltBegin, rotCltEnd, color, lineThick, segments);
			DrawCircleSectorLines(crt, radius, rotCrtBegin, rotCrtEnd, color, lineThick, segments);
			DrawCircleSectorLines(crb, radius, rotCrbBegin, rotCrbEnd, color, lineThick, segments);
			DrawCircleSectorLines(clb, radius, rotClbBegin, rotClbEnd, color, lineThick, segments);
		}
	}

	void DrawCircle(Vec2 center, float radius, Color color)
	{
		::DrawCircleV(RLVec2(center), radius, RLColor(color));
	}

	void DrawCircleLines(Vec2 center, float radius, Color color, float lineThick)
	{
		if (lineThick <= 1.0f)
		{
			::DrawCircleLines(static_cast<int>(lroundf(center.x)), static_cast<int>(lroundf(center.y)), radius, RLColor(color));
		}
		else
		{
			for (float r = radius - lineThick / 2.0f; r <= radius + lineThick / 2.0f; r += 1.0f)
			{
				::DrawCircleLines(static_cast<int>(lroundf(center.x)), static_cast<int>(lroundf(center.y)), r, RLColor(color));
			}
		}
	}

	void DrawCircleGradient(Vec2 pos, float radius, Color color1, Color color2)
	{
		::DrawCircleGradient(RLVec2(pos), radius, RLColor(color1), RLColor(color2));
	}

	void DrawCircleSector(Vec2 center, float radius, float startAngle, float endAngle, Color color, int segments)
	{
		::DrawCircleSector(RLVec2(center), radius, RadToDeg(startAngle), RadToDeg(endAngle), segments, RLColor(color));
	}

	void DrawCircleSectorLines(Vec2 center, float radius, float startAngle, float endAngle, Color color, float lineThick, int segments)
	{
		float halfLineThick = lineThick / 2.0f;
		DrawRing(center, radius - halfLineThick, radius + halfLineThick, startAngle, endAngle, color, segments);
	}

	void DrawRing(Vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, Color color, int segments)
	{
		::DrawRing(RLVec2(center), innerRadius, outerRadius, RadToDeg(startAngle), RadToDeg(endAngle), segments, RLColor(color));
	}

	void DrawRingLines(Vec2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, Color color, float lineThick, int segments)
	{
		float halfLineThick = lineThick / 2.0f;
		DrawRing(center, innerRadius - halfLineThick, innerRadius + halfLineThick, startAngle, endAngle, color, segments);
		DrawRing(center, outerRadius - halfLineThick, outerRadius + halfLineThick, startAngle, endAngle, color, segments);
	}

	void DrawTriangle(Vec2 v1, Vec2 v2, Vec2 v3, Color color)
	{
		::DrawTriangle(RLVec2(v1), RLVec2(v2), RLVec2(v3), RLColor(color));
	}

	void DrawTriangleLines(Vec2 v1, Vec2 v2, Vec2 v3, Color color)
	{
		::DrawTriangleLines(RLVec2(v1), RLVec2(v2), RLVec2(v3), RLColor(color));
	}

	void DrawTriangleFan(const std::vector<Vec2>& pos, Color color)
	{
		std::vector<Vector2> rlPoints;
		rlPoints.reserve(pos.size());
		for (const Vec2& p : pos)
		{
			rlPoints.push_back(RLVec2(p));
		}
		::DrawTriangleFan(rlPoints.data(), static_cast<int>(rlPoints.size()), RLColor(color));
	}

	void DrawTriangleStrip(const std::vector<Vec2>& pos, Color color)
	{
		std::vector<Vector2> rlPoints;
		rlPoints.reserve(pos.size());
		for (const Vec2& p : pos)
		{
			rlPoints.push_back(RLVec2(p));
		}
		::DrawTriangleStrip(rlPoints.data(), static_cast<int>(rlPoints.size()), RLColor(color));
	}

	void DrawPoly(Vec2 center, int sides, float radius, float rotation, Color color)
	{
		::DrawPoly(RLVec2(center), sides, radius, RadToDeg(rotation), RLColor(color));
	}

	void DrawPolyLines(Vec2 center, int sides, float radius, float rotation, Color color, float lineThick)
	{
		::DrawPolyLinesEx(RLVec2(center), sides, radius, RadToDeg(rotation), lineThick, RLColor(color));
	}

	void DrawTexture(const rsc::SharedTexture2D& texture, Vec2 pos, float scale, float rotation, Vec2 pivot, Color tint)
	{
		DrawTexturePro(*static_cast<const Texture2D*>(texture.get()), RLRect(Rect{ 0, 0, static_cast<float>(texture.width()), static_cast<float>(texture.height()) }), RLRect(Rect{ pos, texture.size() }.scaleAround(pos + pivot, scale)), RLVec2(pivot * scale), RadToDeg(rotation), RLColor(tint));
	}

	void DrawTextureRegionToRegion(const rsc::SharedTexture2D& texture, Rect sourceRec, Rect destRec, Vec2 origin, float rotation, Color tint)
	{
		::DrawTexturePro(*static_cast<const Texture2D*>(texture.get()), RLRect(sourceRec), RLRect(destRec), RLVec2(origin), RadToDeg(rotation), RLColor(tint));
	}

	void DrawTextureRegion(const rsc::SharedTexture2D& texture, Rect sourceRec, Vec2 pos, Color tint)
	{
		::DrawTextureRec(*static_cast<const Texture2D*>(texture.get()), RLRect(sourceRec), RLVec2(pos), RLColor(tint));
	}

	void DrawTexture(const rsc::SharedRenderTexture2D& texture, Vec2 pos, float scale, float rotation, Vec2 pivot, Color tint)
	{
		::DrawTexturePro(static_cast<const RenderTexture2D*>(texture.get())->texture, RLRect(Rect{ 0, 0, static_cast<float>(texture.width()), static_cast<float>(-texture.height()) }), RLRect(Rect{ pos, texture.size() }.scaleAround(pos + pivot, scale)), RLVec2(pivot * scale), RadToDeg(rotation), RLColor(tint));//raylib的RenderTexture2D的纹理坐标系以左下角为原点，在此统一修正
	}

	void DrawTextureRegionToRegion(const rsc::SharedRenderTexture2D& texture, Rect sourceRec, Rect destRec, Vec2 origin, float rotation, Color tint)
	{
		::DrawTexturePro(static_cast<const RenderTexture2D*>(texture.get())->texture, RLRect(Rect{ sourceRec.position(), Vec2{ sourceRec.width, -sourceRec.height } }), RLRect(destRec), RLVec2(origin), RadToDeg(rotation), RLColor(tint));
	}

	void DrawTextureRegion(const rsc::SharedRenderTexture2D& texture, Rect sourceRec, Vec2 pos, Color tint)
	{
		::DrawTextureRec(static_cast<const RenderTexture2D*>(texture.get())->texture, RLRect(Rect{ sourceRec.position(), Vec2{ sourceRec.width, -sourceRec.height } }), RLVec2(pos), RLColor(tint));
	}

	void DrawText(const rsc::SharedFont& font, const std::string& text, Vec2 position, float fontSize, float spacing, Color tint, Vec2 pivot, float rotation)
	{
		position += pivot;//修正为pivot语义，以position为原点，origin为旋转中心
		::DrawTextPro(*static_cast<const Font*>(font.get()), text.c_str(), RLVec2(position), RLVec2(pivot), RadToDeg(rotation), fontSize, spacing, RLColor(tint));
	}

	void DrawTextCodepoints(const rsc::SharedFont& font, const std::vector<int32_t>& codepoints, Vec2 position, float fontSize, float spacing, Color tint)
	{
		::DrawTextCodepoints(*static_cast<const Font*>(font.get()), codepoints.data(), static_cast<int>(codepoints.size()), RLVec2(position), fontSize, spacing, RLColor(tint));
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
		ShaderModeGuard guard(shader);
		::DrawTextPro(*static_cast<const Font*>(font.get()), text.c_str(), RLVec2(position), RLVec2(origin), RadToDeg(rotation), fontSize, spacing, RLColor(tint));
	}

	void DrawSDFTextCodepoints(const rsc::SharedFont& font, const std::vector<int32_t>& codepoints, Vec2 position, float fontSize, float spacing, Color tint, const rsc::SharedShader& shader)
	{
		ShaderModeGuard guard(shader);
		::DrawTextCodepoints(*static_cast<const Font*>(font.get()), codepoints.data(), static_cast<int>(codepoints.size()), RLVec2(position), fontSize, spacing, RLColor(tint));
	}

	void DrawLineStrip(const std::vector<Vec2>& points, Color color, float lineThick)
	{
		std::vector<Vector2> rlPoints;
		rlPoints.reserve(points.size());
		for (const Vec2& p : points)
		{
			rlPoints.push_back(RLVec2(p));
		}
		::DrawSplineLinear(rlPoints.data(), static_cast<int>(rlPoints.size()), lineThick, RLColor(color));
	}

	void ClearBackground(Color color)
	{
		::ClearBackground(RLColor(color));
	}
}