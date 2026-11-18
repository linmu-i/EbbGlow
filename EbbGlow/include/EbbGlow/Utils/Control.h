#pragma once


#include <string>

#include <EbbGlow/Resources/Resources.h>
#include <EbbGlow/Utils/Types.h>

namespace ebbglow
{
	namespace flags
	{
		constexpr uint32_t VsyncHint = 0x00000040U;
		constexpr uint32_t FullscreenMode = 0x00000002U;
		constexpr uint32_t WindowResizable = 0x00000004U;
		constexpr uint32_t WindowUndecorated = 0x00000008U;
		constexpr uint32_t WindowHidden = 0x00000080U;
		constexpr uint32_t WindowMinimized = 0x00000200U;
		constexpr uint32_t WindowMaximized = 0x00000400U;
		constexpr uint32_t WindowUnfocused = 0x00000800U;
		constexpr uint32_t WindowTopmost = 0x00001000U;
		constexpr uint32_t WindowAlwaysRun = 0x00000100U;
		constexpr uint32_t WindowTransparent = 0x00000010U;
		constexpr uint32_t WindowHighdpi = 0x00002000U;
		constexpr uint32_t WindowMousePassthrough = 0x00004000U;
		constexpr uint32_t BorderlessWindowedMode = 0x00008000U;
		constexpr uint32_t Msaa4xHint = 0x00000020U;
		constexpr uint32_t InterlacedHint = 0x00010000U;
	}

	void Init(int width, int height, const std::string& title);
	bool WindowShouldClose() noexcept;
	void SetConfigFlag(uint32_t flags);
	void SetTargetFPS(int32_t fps);

	void BeginDrawing();
	void EndDrawing();

	void BeginTextureMode(rsc::SharedRenderTexture2D& renderTexture);
	void EndTextureMode();
	void ClearTextureMode();
	class TextureModeGuard
	{
	private:
		bool active;

	public:
		TextureModeGuard(rsc::SharedRenderTexture2D& renderTexture)
		{
			active = true;
			BeginTextureMode(renderTexture);
		}
		~TextureModeGuard()
		{
			if (active) EndTextureMode();
		}

		TextureModeGuard(const TextureModeGuard&) = delete;
		TextureModeGuard& operator=(const TextureModeGuard&) = delete;
		TextureModeGuard(TextureModeGuard&& other) = delete;
		TextureModeGuard& operator=(TextureModeGuard&& other) = delete;

		void endTextureMode() noexcept
		{
			if (active)
			{
				EndTextureMode();
				active = false;
			}
		}
	};

	void BeginShaderMode(const rsc::SharedShader& shader);
	void EndShaderMode();
	void ClearShaderMode();
	class ShaderModeGuard
	{
	private:
		bool active;

	public:
		ShaderModeGuard(const rsc::SharedShader& renderTexture)
		{
			active = true;
			BeginShaderMode(renderTexture);
		}
		~ShaderModeGuard()
		{
			if (active) EndShaderMode();
		}

		ShaderModeGuard(const ShaderModeGuard&) = delete;
		ShaderModeGuard& operator=(const ShaderModeGuard&) = delete;
		ShaderModeGuard(ShaderModeGuard&& other) = delete;
		ShaderModeGuard& operator=(ShaderModeGuard&& other) = delete;

		void endShaderMode() noexcept
		{
			if (active)
			{
				EndShaderMode();
				active = false;
			}
		}
	};

	void BeginMode2D(const Camera2D& camera2D);
	void EndMode2D();

	class Mode2DGuard
	{
	private:
		bool active;

	public:
		Mode2DGuard(const Camera2D& camera2D)
		{
			active = true;
			BeginMode2D(camera2D);
		}
		~Mode2DGuard()
		{
			if (active) EndMode2D();
		}

		Mode2DGuard(const Mode2DGuard&) = delete;
		Mode2DGuard& operator=(const Mode2DGuard&) = delete;
		Mode2DGuard(Mode2DGuard&& other) = delete;
		Mode2DGuard& operator=(Mode2DGuard&& other) = delete;

		void endMode2D() noexcept
		{
			if (active)
			{
				EndMode2D();
				active = false;
			}
		}
	};

	void BeginScissorMode(Rect region);
	void EndScissorMode();

	class ScissorModeGuard
	{
	private:
		bool active;

	public:
		ScissorModeGuard(Rect region)
		{
			active = true;
			BeginScissorMode(region);
		}
		~ScissorModeGuard()
		{
			if (active) EndScissorMode();
		}

		ScissorModeGuard(const ScissorModeGuard&) = delete;
		ScissorModeGuard& operator=(const ScissorModeGuard&) = delete;
		ScissorModeGuard(ScissorModeGuard&& other) = delete;
		ScissorModeGuard& operator=(ScissorModeGuard&& other) = delete;

		void endScissorMode() noexcept
		{
			if (active)
			{
				EndScissorMode();
				active = false;
			}
		}
	};

	float GetFrameTime();
}