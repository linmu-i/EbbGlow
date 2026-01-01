#pragma once


#include <string>

#include <EbbGlow/Utils/Resource.h>
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

	void BeginDrawing();
	void EndDrawing();

	void BeginTextureMode(rsc::SharedRenderTexture2D renderTexture);
	void EndTextureMode();

    void BeginShaderMode(rsc::SharedShader shader);
    void EndShaderMode();

	void BeginMode2D(const Camera2D& camera2D);
	void EndMode2D();

	float GetFrameTime();
}