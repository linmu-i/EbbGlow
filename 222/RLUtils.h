#pragma once

#include "raylib.h"
#include "raylibRAII.h"
#include "TouchEx.h"
#include <vector>

Font LoadFont_cn(const char* fileName, int fontSize) {
	// 定义需要加载的字符范围
	std::vector<std::pair<int, int>> ranges = {
		// 基本ASCII字符 (32-126)
		{32, 126},

		// 中文常用字符
		{0x4E00, 0x9FFF},   // CJK统一汉字基本区
		{0x3400, 0x4DBF},   // CJK扩展A区
		{0x3000, 0x303F},   // 中文标点符号

		// 全角字符 (包括全角字母、数字、标点)
		{0xFF01, 0xFF5E},   // 全角ASCII变体

		// 额外中文符号支持
		{0x2010, 0x2027},   // 连字符、项目符号等
		{0x2E80, 0x2EFF},   // CJK部首补充
		{0x3000, 0x303F}    // 重复确保覆盖
	};

	// 收集所有码点
	std::vector<int> codepoints;

	for (const auto& range : ranges) {
		for (int i = range.first; i <= range.second; i++) {
			codepoints.push_back(i);
		}
	}

	// 添加额外常用字符（确保覆盖）
	const int extraChars[] = {
		0x00B7,  // 间隔点 (·)
		0x2014,  // 破折号 (—)
		0x2018,  // 左单引号 (‘)
		0x2019,  // 右单引号 (’)
		0x201C,  // 左双引号 (“)
		0x201D,  // 右双引号 (”)
		0x2026,  // 省略号 (…)
		0x3001,  // 顿号 (、)
		0x3002,  // 句号 (。)
		0xFF0D,  // 全角连字符 (－)
		0xFFE5   // 人民币符号 (￥)
	};

	for (int ch : extraChars) {
		codepoints.push_back(ch);
	}

	// 加载字体
	Font font = LoadFontEx(fileName, fontSize, codepoints.data(), static_cast<int>(codepoints.size()));

	// 设置字体纹理过滤为双线性(提高渲染质量)
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	return font;
}

const char* shaderText = R"(
#version 330

in vec2 fragTexCoord;//
in vec4 fragColor; 

out vec4 finalColor0;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main() {
	// 读取SDF纹理的alpha通道（距离场值）
	float sdfValue = texture(texture0, fragTexCoord).a;
	
	// 核心SDF算法：将距离场转换为平滑的alpha值
	float smoothing = 0.02;  // 控制边缘锐利度
	float alpha = smoothstep(0.42 - smoothing, 0.42 + smoothing, sdfValue);
	
	// 应用颜色和透明度
	vec4 texelColor = vec4(fragColor.rgb, fragColor.a * alpha);
	
	// 最终输出
	finalColor0 = texelColor * colDiffuse;
}
)";



Font LoadSDFFontEx(const char* fontInfoFile, const char* textureFile) {
	// 加载文件数据
	int bytesRead = 0;
	unsigned char* data = LoadFileData(fontInfoFile, &bytesRead);

	if (!data || bytesRead < sizeof(int) * 3) {
		TraceLog(LOG_ERROR, "FONT IMPORT: Invalid font data file");
		if (data) UnloadFileData(data);
		return GetFontDefault();
	}

	Font font = { 0 };
	unsigned char* ptr = data;

	// 读取头部
	font.baseSize = *(int*)ptr; ptr += sizeof(int);
	font.glyphCount = *(int*)ptr; ptr += sizeof(int);
	font.glyphPadding = *(int*)ptr; ptr += sizeof(int);

	// 检查数据完整性
	int expectedSize = sizeof(int) * 3 + font.glyphCount * (sizeof(int) * 4 + sizeof(float) * 4);
	if (bytesRead != expectedSize) {
		TraceLog(LOG_ERROR, "FONT IMPORT: Data size mismatch (%d vs %d)", bytesRead, expectedSize);
		UnloadFileData(data);
		return GetFontDefault();
	}

	// 分配内存
	font.recs = (Rectangle*)RL_MALLOC(font.glyphCount * sizeof(Rectangle));
	font.glyphs = (GlyphInfo*)RL_MALLOC(font.glyphCount * sizeof(GlyphInfo));

	// 读取字符数据
	for (int i = 0; i < font.glyphCount; i++) {
		// GlyphInfo 部分
		font.glyphs[i].value = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].offsetX = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].offsetY = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].advanceX = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].image = Image{ 0 }; // 清空图像数据

		// Rectangle 部分
		font.recs[i].x = *(float*)ptr; ptr += sizeof(float);
		font.recs[i].y = *(float*)ptr; ptr += sizeof(float);
		font.recs[i].width = *(float*)ptr; ptr += sizeof(float);
		font.recs[i].height = *(float*)ptr; ptr += sizeof(float);
	}
	// 加载纹理
	font.texture = LoadTexture(textureFile);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR); // SDF必需

	UnloadFileData(data);

	TraceLog(LOG_INFO, "FONT IMPORT: Loaded %d glyphs from %s", font.glyphCount, fontInfoFile);
	return font;
}

namespace fontUtils
{
	static const char* SDFShaderText = R"(
#version 330

in vec2 fragTexCoord;//
in vec4 fragColor; 

out vec4 finalColor0;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main() {
    // 读取SDF纹理的alpha通道（距离场值）
    float sdfValue = texture(texture0, fragTexCoord).a;
    
    // 核心SDF算法：将距离场转换为平滑的alpha值
    float smoothing = 0.01;  // 控制边缘锐利度
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, sdfValue);
    
    // 应用颜色和透明度
    vec4 texelColor = vec4(fragColor.rgb, fragColor.a * alpha);
    
    // 最终输出
    finalColor0 = texelColor * colDiffuse;
}
)";

	
}

void DrawSDFText(Font SDFFont, const char* text, Vector2 position, float fontSize, float spacing, Color tInt)
{
	static rlRAII::ShaderRAII SDFShader{ LoadShaderFromMemory(nullptr, fontUtils::SDFShaderText) };
	BeginShaderMode(SDFShader.get());
	DrawTextEx(SDFFont, text, position, fontSize, spacing, tInt);
	EndShaderMode();
}

std::vector<int> GetUnicodePoints(const char* text) {
	std::vector<int> codePoints;

	while (*text != '\0') {
		uint8_t lead = static_cast<uint8_t>(*text);

		// 单字节字符 (ASCII)
		if (lead < 0x80) {
			codePoints.push_back(lead);
			text++;
			continue;
		}

		// 多字节字符处理
		int numBytes = 0;
		int codePoint = 0;

		// 确定字节数和初始掩码
		if ((lead & 0xE0) == 0xC0) {  // 2字节
			numBytes = 2;
			codePoint = lead & 0x1F;
		}
		else if ((lead & 0xF0) == 0xE0) {  // 3字节
			numBytes = 3;
			codePoint = lead & 0x0F;
		}
		else if ((lead & 0xF8) == 0xF0) {  // 4字节
			numBytes = 4;
			codePoint = lead & 0x07;
		}
		else {
			// 无效的首字节：使用替换字符并跳过
			codePoints.push_back(0xFFFD);
			text++;
			continue;
		}

		// 检查后续字节有效性
		bool valid = true;
		for (int i = 1; i < numBytes; ++i) {
			text++;
			if (*text == '\0' || (static_cast<uint8_t>(*text) < 0x80 || (static_cast<uint8_t>(*text) > 0xBF))) {
				valid = false;
				break;
			}
			codePoint = (codePoint << 6) | (static_cast<uint8_t>(*text) & 0x3F);
		}

		// 处理无效序列
		if (!valid) {
			// 回退到错误发生点
			text -= (numBytes - 1) - 1;  // 回到错误字节位置
			codePoints.push_back(0xFFFD);
			text++;
			continue;
		}

		// 验证码点范围
		bool rangeValid = true;
		if (numBytes == 2 && codePoint < 0x80) rangeValid = false;
		else if (numBytes == 3 && codePoint < 0x800) rangeValid = false;
		else if (numBytes == 4 && (codePoint < 0x10000 || codePoint > 0x10FFFF)) rangeValid = false;

		if (rangeValid) {
			codePoints.push_back(codePoint);
		}
		else {
			codePoints.push_back(0xFFFD);
		}

		text++;  // 移动到下一个字符
	}

	return codePoints;
}

#include <unordered_set>

std::vector<int> GetUnicodePointsUnique(const char* text) {
	std::vector<int> codePoints;
	std::unordered_set<int> seen;  //新增：记录已见码点

	while (*text != '\0') {
		uint8_t lead = static_cast<uint8_t>(*text);

		// 单字节字符 (ASCII)
		if (lead < 0x80) {
			int cp = lead;
			//去重判断
			if (seen.find(cp) == seen.end()) {
				seen.insert(cp);
				codePoints.push_back(cp);
			}
			text++;
			continue;
		}

		// 多字节字符处理
		int numBytes = 0;
		int codePoint = 0;

		if ((lead & 0xE0) == 0xC0) {  // 2字节
			numBytes = 2;
			codePoint = lead & 0x1F;
		}
		else if ((lead & 0xF0) == 0xE0) {  // 3字节
			numBytes = 3;
			codePoint = lead & 0x0F;
		}
		else if ((lead & 0xF8) == 0xF0) {  // 4字节
			numBytes = 4;
			codePoint = lead & 0x07;
		}
		else {
			// 无效首字节：使用替换字符并跳过
			int cp = 0xFFFD;
			if (seen.find(cp) == seen.end()) {
				seen.insert(cp);
				codePoints.push_back(cp);
			}
			text++;
			continue;
		}

		// 检查后续字节有效性
		bool valid = true;
		for (int i = 1; i < numBytes; ++i) {
			text++;
			if (*text == '\0' || (static_cast<uint8_t>(*text) < 0x80 || (static_cast<uint8_t>(*text) > 0xBF))) {
				valid = false;
				break;
			}
			codePoint = (codePoint << 6) | (static_cast<uint8_t>(*text) & 0x3F);
		}

		if (!valid) {
			text -= (numBytes - 1) - 1;
			int cp = 0xFFFD;
			if (seen.find(cp) == seen.end()) {
				seen.insert(cp);
				codePoints.push_back(cp);
			}
			text++;
			continue;
		}

		// 验证码点范围
		bool rangeValid = true;
		if (numBytes == 2 && codePoint < 0x80) rangeValid = false;
		else if (numBytes == 3 && codePoint < 0x800) rangeValid = false;
		else if (numBytes == 4 && (codePoint < 0x10000 || codePoint > 0x10FFFF)) rangeValid = false;

		int cp = rangeValid ? codePoint : 0xFFFD;

		//关键：仅当首次出现时加入
		if (seen.find(cp) == seen.end()) {
			seen.insert(cp);
			codePoints.push_back(cp);
		}

		text++;  // 移动到下一个字符
	}

	return codePoints;
}

Font DynamicLoadFont(const char* text, const char* fontPath, float fontSize)
{
	int codepointCount = 0;
	//std::vector<int> codepoints = GetUnicodePoints(text);
	int* codepoints = LoadCodepoints(text, &codepointCount);
	Font font = LoadFontEx(fontPath, fontSize, codepoints, codepointCount);
	UnloadCodepoints(codepoints);
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
	return font;
}

Font DynamicLoadFontFromMemory(const char* text, const char* fileName, const unsigned char* fontData, int dataSize, float fontSize)
{
	//int codepointCount = 0;
	std::vector<int> codepoints = GetUnicodePoints(text);
	codepoints.insert(codepoints.end(), codepoints.begin(), codepoints.end());
	//int* codepoints = LoadCodepoints(text, &codepointCount);
	const char* p = fileName;
	while (*p != '\0') 
	{
		++p;
	}
	do
	{
		--p;
	} while (*p != '.' && p != fileName);
	//Font font = {};
	//font = LoadFontFromMemory(p, fontData, dataSize, fontSize, codepoints, codepointCount);
	Font font = LoadFontFromMemory(p, fontData, dataSize, fontSize, codepoints.data(), codepoints.size());
	//UnloadCodepoints(codepoints);
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
	return font;
}



std::string GenerateGaussianBlurShaderCode(int radius)
{
	// 计算高斯核权重
	std::vector<float> weights;
	float sum = 0.0f;

	for (int i = -radius; i <= radius; i++)
	{
		float weight = std::exp(-(i * i) / (2.0f * radius * radius));
		weights.push_back(weight);
		sum += weight;
	}

	// 归一化权重
	for (auto& weight : weights)
	{
		weight /= sum;
	}

	// 构建着色器代码
	std::string shaderCode = R"(
#version 330

// 输入 uniforms
uniform sampler2D inputTexture;
uniform vec2 direction;
uniform vec2 textureSize;

// 输入顶点数据
in vec2 fragTexCoord;
out vec4 finalColor;

// 高斯核权重
const float weights[)";

	shaderCode += std::to_string(weights.size());
	shaderCode += "] = float[";
	shaderCode += std::to_string(weights.size());
	shaderCode += "](";

	for (size_t i = 0; i < weights.size(); i++)
	{
		shaderCode += std::to_string(weights[i]);
		if (i < weights.size() - 1)
			shaderCode += ", ";
	}

	shaderCode += R"();

void main()
{
    vec2 texOffset = 1.0 / textureSize;
    vec4 result = vec4(0.0);
    
    for (int i = -)";

	shaderCode += std::to_string(radius);
	shaderCode += "; i <= ";
	shaderCode += std::to_string(radius);
	shaderCode += R"(; ++i)
    {
        vec2 offset = texOffset * float(i) * direction;
        result += texture(inputTexture, fragTexCoord + offset) * weights[i + )";
	shaderCode += std::to_string(radius);
	shaderCode += R"(];
    }
    
    finalColor = result;
}
)";

	return shaderCode;
}

// 高斯模糊函数 - 支持可调半径
rlRAII::Texture2DRAII TextureBlurGaussian(const Texture2D& inputTexture, int radius)
{

	// 验证半径值
	if (radius < 1) radius = 1;
	if (radius > 20) radius = 20; // 设置上限防止性能问题

	// 创建两个RenderTexture，避免读写冲突
	RenderTexture2D horizontalBlurRT = LoadRenderTexture(inputTexture.width, inputTexture.height);
	RenderTexture2D verticalBlurRT = LoadRenderTexture(inputTexture.width, inputTexture.height);


	// 设置纹理环绕模式为CLAMP，防止边缘采样问题
	SetTextureWrap(horizontalBlurRT.texture, TEXTURE_WRAP_CLAMP);
	SetTextureWrap(verticalBlurRT.texture, TEXTURE_WRAP_CLAMP);

	// 生成着色器代码
	std::string shaderCode = GenerateGaussianBlurShaderCode(radius);

	// 从内存加载着色器
	Shader blurShader = LoadShaderFromMemory(nullptr, shaderCode.c_str());

	// 获取着色器uniform位置
	int directionLoc = GetShaderLocation(blurShader, "direction");
	int textureSizeLoc = GetShaderLocation(blurShader, "textureSize");
	int inputTextureLoc = GetShaderLocation(blurShader, "inputTexture");


	// 设置纹理尺寸uniform
	float textureSize[2] = { (float)inputTexture.width, (float)inputTexture.height };
	SetShaderValue(blurShader, textureSizeLoc, textureSize, SHADER_UNIFORM_VEC2);

	// 第一遍: 水平模糊
	float horizontal[2] = { 1.0f, 0.0f };
	SetShaderValue(blurShader, directionLoc, horizontal, SHADER_UNIFORM_VEC2);

	// 渲染到水平模糊RenderTexture
	BeginTextureMode(horizontalBlurRT);
	ClearBackground(BLANK);
	BeginShaderMode(blurShader);

	// 设置输入纹理
	SetShaderValueTexture(blurShader, inputTextureLoc, inputTexture);

	// 绘制全屏矩形应用着色器
	DrawTexture(inputTexture, 0, 0, WHITE);

	EndShaderMode();
	EndTextureMode();

	// 第二遍: 垂直模糊
	float vertical[2] = { 0.0f, 1.0f };
	SetShaderValue(blurShader, directionLoc, vertical, SHADER_UNIFORM_VEC2);

	// 渲染到垂直模糊RenderTexture（使用水平模糊的结果作为输入）
	BeginTextureMode(verticalBlurRT);
	ClearBackground(BLANK);
	BeginShaderMode(blurShader);

	// 使用水平模糊的结果作为输入
	SetShaderValueTexture(blurShader, inputTextureLoc, horizontalBlurRT.texture);

	// 绘制全屏矩形应用着色器
	DrawTexture(horizontalBlurRT.texture, 0, 0, WHITE);

	EndShaderMode();
	EndTextureMode();

	// 关键步骤: 提取纹理并置空RenderTexture的纹理引用
	Texture2D blurredTexture = verticalBlurRT.texture;
	verticalBlurRT.texture = {}; // 置空，防止UnloadRenderTexture释放纹理

	// 卸载不再需要的资源
	UnloadShader(blurShader);
	UnloadRenderTexture(horizontalBlurRT);
	UnloadRenderTexture(verticalBlurRT); // 这会释放帧缓冲区，但不会释放纹理

	// 使用提取的纹理创建RAII对象
	return rlRAII::Texture2DRAII(blurredTexture);
}

std::string CodepointToUtf8(int codepoint) {
	std::string result;

	if (codepoint <= 0x7F) {
		// 1 字节 UTF-8
		result += static_cast<char>(codepoint);
	}
	else if (codepoint <= 0x7FF) {
		// 2 字节 UTF-8
		result += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
		result += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0xFFFF) {
		// 3 字节 UTF-8
		result += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
		result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		result += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0x10FFFF) {
		// 4 字节 UTF-8
		result += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
		result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
		result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		result += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else {
		// 无效的 Unicode 码点
		result += '?';
	}

	return result;
}

// 将码点数组转换为 UTF-8 字符串 (std::string 版本)
std::string CodepointsToString(int* codepoints, int codepointsCount) {
	std::string result;

	for (int i = 0; i < codepointsCount; i++) {
		result += CodepointToUtf8(codepoints[i]);
	}

	return result;
}

std::vector<std::vector<int>> TextLineCaculateWithWordWrap(std::string text, float fontSize, float spacing, const Font& font, float maxLength)
{
	int codepointsCount;
	int* codepoints = LoadCodepoints(text.c_str(), &codepointsCount);

	std::vector<std::vector<int>> result;
	int begin = 0;

	while (begin < codepointsCount)
	{
		int tmpCount = codepointsCount - begin;
		float lineLengthTmp;
		int lastSpace = -1; // 记录最后一个空格的位置

		// 首先尝试找到最后一个空格位置
		for (int i = 0; i < tmpCount; i++)
		{
			if (codepoints[begin + i] == ' ' || codepoints[begin + i] == '\n')
			{
				lastSpace = i;
			}
		}

		// 测量当前行的长度
		do
		{
			std::string lineText = CodepointsToString(codepoints + begin, tmpCount);
			lineLengthTmp = MeasureTextEx(font, lineText.c_str(), fontSize, spacing).x;

			if (lineLengthTmp > maxLength)
			{
				// 如果有空格并且可以在空格处断开
				if (lastSpace != -1 && lastSpace < tmpCount)
				{
					tmpCount = lastSpace + 1; // 包括空格
					break;
				}
				else if (tmpCount > 1)
				{
					tmpCount--;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		} while (true);

		// 处理特殊情况
		if (tmpCount == 0 && begin < codepointsCount)
		{
			tmpCount = 1;
		}

		// 跳过行首空格
		while (tmpCount > 0 && codepoints[begin] == ' ')
		{
			begin++;
			tmpCount--;
		}

		// 跳过行尾空格
		while (tmpCount > 0 && codepoints[begin + tmpCount - 1] == ' ')
		{
			tmpCount--;
		}

		if (tmpCount > 0)
		{
			std::vector<int> line(codepoints + begin, codepoints + begin + tmpCount);
			result.push_back(line);
		}

		begin += tmpCount;

		// 跳过下一行的开头空格
		while (begin < codepointsCount && codepoints[begin] == ' ')
		{
			begin++;
		}
	}

	UnloadCodepoints(codepoints);
	return result;
}

bool IsTouchPress() noexcept
{
	static int count = 0;
	int tmp = GetTouchPointCountEx();
	if (count == 0 && tmp > 0)
	{
		count = tmp;
		return true;
	}
	else
	{
		count = tmp;
		return false;
	}
}