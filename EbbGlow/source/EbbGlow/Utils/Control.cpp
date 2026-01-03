#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Input.h>
//#include <glad/glad.h>

#include "RLTypesCast.h"

#include <string>
#include <raylib.h>

namespace ebbglow
{
	void Init(int width, int height, const std::string& title)
	{
		::InitWindow(width, height, title.c_str());
		InitAudioDevice();
		input::InitInput();
	}
	bool WindowShouldClose() noexcept
	{
		return ::WindowShouldClose();
	}
	void SetConfigFlag(uint32_t flags)
	{
		::SetConfigFlags(flags);
	}

	void BeginDrawing()
	{
		::BeginDrawing();
	}
	void EndDrawing()
	{
		input::SwapActiveInputBuffers();
		::EndDrawing();
	}

	void BeginTextureMode(rsc::SharedRenderTexture2D renderTexture)
	{
		::BeginTextureMode(*(RenderTexture*)renderTexture.get());
	}
	void EndTextureMode()
	{
		::EndTextureMode();
	}

	void BeginShaderMode(rsc::SharedShader shader)
	{
		::BeginShaderMode(*(Shader*)shader.get());
	}
	void EndShaderMode()
	{
		::EndShaderMode();
	}

	void BeginMode2D(const Camera2D& camera2D)
	{
		::BeginMode2D(RLCamera2D(camera2D));
	}
	void EndMode2D()
	{
		::EndMode2D();
	}

	float GetFrameTime()
	{
		return ::GetFrameTime();
	}
}