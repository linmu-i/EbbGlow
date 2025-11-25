#define _CRT_SECURE_NO_WARNINGS


#include "ui/Animation.h"
#include "visualnovel/VisualNovel.h"
#include "visualnovel/ScriptLoader.h"

#include <stdlib.h>
#include <rlgl.h>

//onst char* FONT_PATH = R"(C:\Windows\Fonts\Dengb.ttf)";
const char* FONT_PATH = R"(resource\font\Noto_Sans_SC\static\NotoSansSC-SemiBold.ttf)";
//const char* FONT_PATH = R"(C:\Windows\Fonts\msyh.ttf)";

rsc::SharedFile FontData(FONT_PATH);

constexpr int WinWidth = 1920;
constexpr int WinHeight = 1080;

visualnovel::VisualNovelConfig CFG;

int main()
{
	SetConfigFlags(FLAG_WINDOW_UNDECORATED);// | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(GetScreenWidth(), GetScreenHeight(), "");
	SetWindowPosition(0, 0);
	InitAudioDevice();
	InitTouchEx();

	SetExitKey(KEY_ESCAPE);

	CFG.textSpeed = 1.0f;
	CFG.fontData = rsc::SharedFile(FONT_PATH);
	CFG.textBoxBackGround = rsc::SharedTexture2D("resource\\img\\TextBoxBackground.png");
	CFG.chrNameBackGround = rsc::SharedTexture2D("resource\\img\\ChrBoxBackground.png");
	CFG.textSize = 36;
	CFG.readTextColor = { 170, 230, 255, 255 };
	CFG.chrNameOffsetX = 0.5f;
	CFG.mainLanguage = 0;
	CFG.secondaryLanguage = 2;

	auto ScX = GetScreenWidth();
	auto ScY = GetScreenHeight();

	if (ScX / ScY > 16.0f / 9.0f)
	{
		CFG.ScreenHeight = ScY;
		CFG.ScreenWidth = int(ScY * (16.0f / 9.0f));
		CFG.drawOffset = { (ScX - CFG.ScreenWidth) / 2.0f, 0.0f };
	}
	else
	{
		CFG.ScreenWidth = ScX;
		CFG.ScreenHeight = int(ScX / (16.0f / 9.0f));
		CFG.drawOffset = { 0.0f, (ScY - CFG.ScreenHeight) / 2.0f };
	}
	CFG.readTextSet = std::unordered_set<std::string>();

	ecs::World2D main(CFG.ScreenWidth, CFG.ScreenHeight);

	rsc::SharedFile sc("Script.txt");
	vn::ScriptLoader scLoader(main, "Script.txt", CFG);
	auto task = scLoader.load();
	vn::ApplyScriptLoader(main, scLoader, CFG);
	
	task.get();
	scLoader.start();

	while (!WindowShouldClose())
	{
		main.update();

		BeginDrawing();
		ClearBackground(BLACK);
		main.draw();
		DrawFPS(10, 10);
		EndDrawing();
	}
	return 0;
}