#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

#include <EbbGlow/Utils/Functions.h>

#include <raylib.h>
#include "ResourceCreator.h"

namespace ebbglow::utils
{
	Vec2 MeasureTextSize(rsc::SharedFont font, const std::string& text, float textSize, float spacing) noexcept
	{
		Vector2 result = MeasureTextEx(*(static_cast<Font*>(font.get())), text.c_str(), textSize, spacing);
		return { result.x, result.y };
	}

	///**;--%{'#"""<!-- SDF功能待优化 -->"""*/%}
	rsc::SharedFont DynamicLoadFont(const rsc::SharedFile& fontData, const std::string& text, float fontSize, rsc::FontType type) noexcept
	{
		if (!fontData.valid()) return rsc::SharedFont();
		const char* p = fontData.fileName();
		while (*p != '\0') ++p;
		while (*p != '.' && p != fontData.fileName()) --p;
		if (*p != '.') return rsc::SharedFont();
		int codePointsCount;
		int* codePoints = ::LoadCodepoints(text.c_str(), &codePointsCount);
		std::vector<int> uniqueCodePoints;
		{
			std::unordered_set<int> seen;
			for (int i = 0; i < codePointsCount; ++i) {
				if (seen.insert(codePoints[i]).second) { // 插入成功=未见过
					uniqueCodePoints.push_back(codePoints[i]);
				}
			}
		}
		UnloadCodepoints(codePoints);

		rsc::ResourceCreator creator;
		if (type == rsc::FontType::Default)
		{
			Font font = LoadFontFromMemory(p, fontData.get(), fontData.size(), fontSize, uniqueCodePoints.data(), uniqueCodePoints.size());
			//SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
			rsc::SharedFont result = creator.CreateFont(font);
			return result;
		}
		else if (type == rsc::FontType::Sdf)
		{
			if (strcmp(p, ".ttf"))
			{
				return rsc::SharedFont();
			}
			Font sdfFont;
			sdfFont.baseSize = fontSize;
			sdfFont.glyphs = LoadFontData(fontData.get(), fontData.size(), fontSize, uniqueCodePoints.data(), uniqueCodePoints.size(), FONT_SDF);
			sdfFont.glyphCount = uniqueCodePoints.size();
			Image atlas = GenImageFontAtlas(sdfFont.glyphs, &sdfFont.recs, uniqueCodePoints.size(), fontSize, 2, 1);
			sdfFont.texture = LoadTextureFromImage(atlas);
			sdfFont.glyphPadding = 2;
			UnloadImage(atlas);
			SetTextureFilter(sdfFont.texture, TEXTURE_FILTER_BILINEAR);
			return creator.CreateFont(sdfFont);
		}
	}

	struct WidthCache {
		const Font& font;
		float fontSize;
		float spacing;
		mutable std::unordered_map<int, float> cache; // mutable 允许 const 方法修改

		WidthCache(const Font& f, float fs, float sp) : font(f), fontSize(fs), spacing(sp) {}

		// 返回单个字符的 advance 宽度（不含后续 spacing）
		float getWidth(int codepoint) const {
			auto it = cache.find(codepoint);
			if (it != cache.end()) return it->second;

			float width = 0.0f;

			// 尝试用 GetGlyphInfo（Raylib 4.2+）
			GlyphInfo gi = GetGlyphInfo(font, codepoint);
			if (gi.advanceX > 0) {
				// Raylib 的 GlyphInfo.advanceX 是 0..1 范围？不，是像素单位，但基于 font.baseSize
				// 实际宽度 = advanceX * (fontSize / font.baseSize)
				width = gi.advanceX * fontSize / static_cast<float>(font.baseSize);
			}
			else {
				// Fallback: 构造单字符 UTF-8 并测量（仅一次）
				char* u8 = LoadUTF8(&codepoint, 1);
				if (u8) {
					Vector2 size = MeasureTextEx(font, u8, fontSize, 0.0f); // 注意：spacing 单独加！
					width = size.x;
					UnloadUTF8(u8);
				}
				else {
					width = 0.0f;
				}
			}

			// 缓存（线程不安全，如需并发可加 mutex，但排版通常单线程）
			cache[codepoint] = width;
			return width;
		}

		// 计算一段 [begin, end) 的总宽度（含 spacing）
		float measureRange(const int* cp, int begin, int end) const {
			if (begin >= end) return 0.0f;
			float sum = 0.0f;
			for (int i = begin; i < end; ++i) {
				sum += getWidth(cp[i]);
				if (i > begin) sum += spacing; // 第一个字符后才加 spacing
			}
			return sum;
		}
	};

	std::vector<std::vector<int>> TextLineCalculateWithWordWrap(
		const std::string& text,
		float fontSize,
		float spacing,
		const rsc::SharedFont& font,
		float maxLength) noexcept
	{
		if (text.empty() || maxLength <= 0.0f) {
			return {};
		}

		// Step 1: 加载 Codepoints
		int codepointsCount = 0;
		int* codepoints = ::LoadCodepoints(text.c_str(), &codepointsCount);
		if (codepointsCount <= 0 || !codepoints) {
			return {};
		}

		// Step 2: 构建宽度缓存
		WidthCache wcache(*static_cast<Font*>(font.get()), fontSize, spacing);

		std::vector<std::vector<int>> result;
		int begin = 0;

		while (begin < codepointsCount) {
			// === 1. 查找最近的硬换行符 '\n' ===
			int hardBreak = -1;
			for (int i = begin; i < codepointsCount; ++i) {
				if (codepoints[i] == '\n') {
					hardBreak = i;
					break;
				}
			}

			// === 2. 确定初始断点 ===
			int end = codepointsCount;
			if (hardBreak != -1) {
				end = hardBreak + 1; // 包含 '\n'
			}

			// === 3. 测量当前段，若超长则尝试软断 ===
			float lineWidth = wcache.measureRange(codepoints, begin, end);
			if (lineWidth > maxLength) {
				// 情况 A: 有硬换行但超长 → 忽略硬换行，尝试软断（实际罕见）
				// 情况 B: 无硬换行 → 尝试找空格或强制截断

				// 先找 [begin, min(end, codepointsCount)) 中最后一个空格
				int lastSpace = -1;
				int limit = std::min(end, codepointsCount);
				for (int i = begin; i < limit; ++i) {
					if (codepoints[i] == ' ') {
						lastSpace = i;
					}
				}

				if (lastSpace != -1) {
					// 尝试断在空格后
					end = lastSpace + 1;
					lineWidth = wcache.measureRange(codepoints, begin, end);
				}

				// 若仍超长（或无空格），强制贪心截断
				if (lineWidth > maxLength) {
					end = begin;
					lineWidth = 0.0f;
					// 贪心添加字符直到再加一个就超
					while (end < codepointsCount) {
						float nextWidth = lineWidth
							+ wcache.getWidth(codepoints[end])
							+ (end > begin ? spacing : 0.0f);
						if (nextWidth > maxLength) break;
						lineWidth = nextWidth;
						end++;
					}
					// 至少保留一个字符（防死循环）
					if (end == begin && begin < codepointsCount) {
						end = begin + 1;
					}
				}
			}

			// === 4. Trim 首尾空格（不影响原始索引）===
			int trimBegin = begin;
			int trimEnd = end;

			// 跳过行首空格
			while (trimBegin < trimEnd && codepoints[trimBegin] == ' ') {
				trimBegin++;
			}
			// 跳过行尾空格（注意：'\n' 已包含在 [begin, end) 中，但 trim 时应去掉）
			while (trimEnd > trimBegin &&
				(codepoints[trimEnd - 1] == ' ' || codepoints[trimEnd - 1] == '\n')) {
				trimEnd--;
			}

			// === 5. 压入非空行 ===
			if (trimEnd > trimBegin) {
				result.emplace_back(codepoints + trimBegin, codepoints + trimEnd);
			}

			// === 6. 更新 begin ===
			begin = end;

			// 跳过刚处理的 '\n'（如果 end 止于 '\n'）
			if (begin > 0 && begin <= codepointsCount && codepoints[begin - 1] == '\n') {
				// 已包含 \n，前进一位
				// （注意：trim 时已去除，但 begin 仍指向 \n 后）
				// 实际上：若 end = i+1 且 codepoints[i]=='\n'，则 begin = i+1，已跳过
			}

			// 跳过下一行开头的空格（为下一轮准备）
			while (begin < codepointsCount && codepoints[begin] == ' ') {
				begin++;
			}
		}
		
		UnloadCodepoints(codepoints);
		return result;
	}

	//警告，会破坏RenderTexture，调用请确保RenderTexture无其他副本
	rsc::SharedTexture2D TakeTextureFromRenderTexture(rsc::SharedRenderTexture2D&& renderTexture) noexcept
	{
		if (!renderTexture.valid()) return rsc::SharedTexture2D();
		::Texture2D texture = static_cast<RenderTexture2D*>(renderTexture.get())->texture;
		static_cast<RenderTexture2D*>(renderTexture.get())->texture = {};
		rsc::SharedRenderTexture cleanup = std::move(renderTexture);
		static rsc::ResourceCreator creator;
		SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
		rsc::SharedTexture2D result = creator.CreateTexture(texture);
		return result;
	}

	float GetFrameTime() noexcept
	{
		return ::GetFrameTime();
	}

	std::string ToUTF8Text(const std::vector<int32_t>& codepoints) noexcept
	{
		char* u8 = LoadUTF8(codepoints.data(), codepoints.size());
		std::string result(u8);
		UnloadUTF8(u8);
		return result;
	}
	std::vector<int32_t> ToCodepoints(const std::string& text) noexcept
	{
		int size;
		int32_t* cp = ::LoadCodepoints(text.c_str(), &size);
		std::vector<int32_t> result(cp, cp + size);
		UnloadCodepoints(cp);
		return result;
	}

	Vec2 ScreenSize() noexcept
	{
		return { static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) };
	}
}