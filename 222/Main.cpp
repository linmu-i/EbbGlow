#define _CRT_SECURE_NO_WARNINGS

#include "World.h"
#include "UI.h"
#include "Animation.h"
#include "RLUtils.h"
#include "VisualNovel.h"

#include <stdlib.h>
#include <rlgl.h>

//onst char* FONT_PATH = R"(C:\Windows\Fonts\Dengb.ttf)";
const char* FONT_PATH = R"(resource\font\Noto_Sans_SC\static\NotoSansSC-SemiBold.ttf)";
//const char* FONT_PATH = R"(C:\Windows\Fonts\msyh.ttf)";

rlRAII::FileRAII FontData(FONT_PATH);

constexpr int WinWidth = 1920;
constexpr int WinHeight = 1080;

visualnovel::VisualNovelConfig CFG;

/*
enum class AllStates : uint8_t
{
	Menu,
	Main,
	Config
};

class MenuWorld : public ecs::World2D
{
private:
	rlRAII::MusicRAII music;
	AllStates& state;
	int buttonStart;
	int buttonConfig;
	int background;
	int buttonExId;
	int animationId;
	float& volume;

	class System : public ecs::SystemBase
	{
	private:
		AllStates& state;
		float& volume;
		int start;
		int config;
		ecs::MessageManager& msgMgr;
		rlRAII::MusicRAII music;

	public:
		System(ecs::MessageManager* msgMgr, AllStates& state, int start, int config, rlRAII::MusicRAII& music, float& volume) : msgMgr(*msgMgr), state(state), start(start), config(config), music(music), volume(volume) {}

		void update()
		{
			SetMusicVolume(music.get(), volume);
			UpdateMusicStream(music.get());
			auto s = msgMgr.getMessageList(start);
			auto c = msgMgr.getMessageList(config);
			if (s != nullptr)
			{
				if (s->size() > 0)
				{
					state = AllStates::Main;
				}
			}
			if (c != nullptr)
			{
				if (c->size() > 0)
				{
					state = AllStates::Config;
				}
			}
		}
	};

public:
	MenuWorld(AllStates& state, int scrX, int scrY, float& volume) : state(state), music("resource\\music\\1.mp3"), World2D(scrX, scrY), volume(volume)
	{
		PlayMusicStream(music.get());
		ui::ApplyButton(*this);
		ui::ApplyImageBox(*this);
		ui::ApplyButtonEx(*this);
		ui::ApplyAnimation(*this);
		ui::ApplyTextBoxEx(*this);
		buttonStart = this->getEntityManager()->getId();
		buttonConfig = this->getEntityManager()->getId();
		background = this->getEntityManager()->getId();
		buttonExId = this->getEntityManager()->getId();
		animationId = this->getEntityManager()->getId();

		rlRAII::Texture2DRAII baseIcon = LoadTexture(u8"resource\\img\\按钮基本.png");
		rlRAII::Texture2DRAII hoverIcon = LoadTexture("resource\\img\\按钮悬浮.png");
		rlRAII::Texture2DRAII pressIcon = LoadTexture("resource\\img\\按钮按下.png");

		this->createUnit(buttonStart, ui::ButtonCom({ 200,650 }, 300, 80, 50, WHITE, BLUE, "Start", nullptr));
		this->createUnit(buttonConfig, ui::ButtonCom({ 200,750 }, 300, 80, 50, WHITE, BLUE, "Config", nullptr));
		this->createUnit(buttonExId, ui::ButtonExCom(FontData, baseIcon, hoverIcon, pressIcon, "", WHITE, 50, 3, { 200,850 }, { 150,50 }, 8, 1.0f));
		this->createUnit(background, ui::ImageBoxCom{ {0,0}, rlRAII::Texture2DRAII("resource\\img\\backGround.png") });
		this->createUnit(animationId, ui::AnimationCom(
			{ "resource\\animation\\z1.png", "resource\\animation\\z2.png", "resource\\animation\\z3.png", "resource\\animation\\z4.png", "resource\\animation\\z5.png" },
			0.5f, { 200,950 }, this->getUiLayer(), 9));
		
		this->getSystem<ui::AnimationSystem>()->loop(animationId, true);
		this->getSystem<ui::AnimationSystem>()->play(animationId);

		this->getSystem<ui::ButtonSystem>()->registerListener(buttonStart, buttonStart);
		this->getSystem<ui::ButtonSystem>()->registerListener(buttonConfig, buttonConfig);

		this->addSystem(System(this->getMessageManager(), state, buttonStart, buttonConfig, music, volume));
	}


};

class Config : public ecs::World2D
{
private:
	AllStates& state;
	bool& showFPS;

	int back;
	int select;
	int text;
	int musicDynamicSlider;
	int mscVlm;

	rlRAII::Texture2DRAII bg;
	bool initialized = false;
	float timeCount = 0.0f;
	int blurSize = 0;

	class System : public ecs::SystemBase
	{
	private:
		AllStates& state;
		int back;
		int select;
		int slider;
		ecs::MessageManager& msgMgr;
		bool& showFPS;
		float& timeCount;
		float& volume;
		ecs::World2D* wld;

	public:
		System(ecs::MessageManager* msgMgr, AllStates& state, int back, int select, int slider, bool& showFPS, float& volume, ecs::World2D* wld, bool& init, float& timeCount) : msgMgr(*msgMgr), state(state), back(back), select(select), wld(wld), showFPS(showFPS), timeCount(timeCount), slider(slider), volume(volume) {}

		void update()
		{
			auto b = msgMgr.getMessageList(back);
			auto s = wld->getDoubleBuffer<ui::SwitchCom>()->active()->get(select);
			volume = wld->getDoubleBuffer<ui::SliderCom>()->active()->get(slider)->value * 2.0f;
			if (b != nullptr)
			{
				if (b->size() > 0)
				{
					state = AllStates::Menu;
					//init = false;
					timeCount = 0.0f;
				}
			}
			if (s != nullptr)
			{
				if (s->state)
				{
					showFPS = true;
				}
				else
				{
					showFPS = false;
				}
			}
		}
	};

	class Dr : public ecs::DrawBase
	{
	public:
		void draw() override
		{
			DrawCircle(1010, 536, 10, WHITE);
		}
	};
	
	class BG : public ecs::DrawBase
	{
	private:
		rlRAII::Texture2DRAII bg;

	public:
		BG(rlRAII::Texture2DRAII bg) : bg(bg) {}
		
		void draw() override
		{
			//DrawTexture(bg.get(), 0, 0, WHITE);0
			DrawTextureRec(bg.get(), Rectangle{ 0,0,float(bg.get().width), float(-bg.get().height) }, { 0,0 }, WHITE);
		}
	};

	class Sys : public ecs::SystemBase
	{
	private:
		ecs::Layers& layer;
		rlRAII::Texture2DRAII& bg;

	public:
		Sys(ecs::Layers* layer, rlRAII::Texture2DRAII& bg) : layer(*layer), bg(bg) {}

		void update() override
		{
			//layer[1].push_back(std::make_unique<Dr>(Dr()));
			layer[0].push_back(std::make_unique<BG>(bg));
		}
	};

public:
	Config(AllStates& state, bool& showFPS, float& volume, int scrX, int scrY) : state(state), showFPS(showFPS), World2D(scrX, scrY)
	{
		ui::ApplyButton(*this);
		ui::ApplySwitch(*this);
		ui::ApplySlider(*this);
		ui::ApplyTextBoxEx(*this);

		back = this->getEntityManager()->getId();
		select = this->getEntityManager()->getId();
		text = this->getEntityManager()->getId();
		mscVlm = this->getEntityManager()->getId();
		musicDynamicSlider = this->getEntityManager()->getId();

		this->createUnit(back, ui::ButtonCom({ 150,150 }, 300, 80, 50, WHITE, BLUE, "Back", nullptr));
		this->createUnit(select, ui::SwitchCom(15, {0xaa, 0xaa, 0xff, 0xff}, { 550, 530 }, 3));//ui::ButtonCom({ 1000,500 }, 20, 20, 100, BLACK, nullptr, ".", nullptr));
		//this->createUnit(text, ui::ButtonCom({ 850,537 }, 0, 0, 50, WHITE, BLUE, "ShowFPS", nullptr));
		this->createUnit(text, ui::TextBoxExCom(FontData, u8"显示帧率: ", { 350,500 }, WHITE, 5, 50, 5));
		this->createUnit(mscVlm, ui::TextBoxExCom(FontData, u8"音量调节: ", { 850,500 }, WHITE, 5, 50, 5));
		this->createUnit(musicDynamicSlider, ui::SliderCom(300, 0.5f, 1.0f, 0, { 1200,530 }, { 0xaa,0xaa,0xaa,0xff }, { 0xaa,0xaa,0xff,0xff }, nullptr, nullptr, 3));

		this->getSystem<ui::ButtonSystem>()->registerListener(back, back);
		this->getSystem<ui::ButtonSystem>()->registerListener(select, select);

		this->addSystem(System(this->getMessageManager(), state, back, select, musicDynamicSlider, showFPS, volume, this, initialized, timeCount));
		this->addSystem(Sys(this->getUiLayer(), bg));
	}

	void init(const Texture2D background)
	{
		if (timeCount < 0.6f && (timeCount == 0.0f || blurSize != int(timeCount * 15 / 0.6)))
		{
			blurSize = int(timeCount * 15 / 0.6);
			bg = TextureBlurGaussian(background, blurSize);
			initialized = true;
		}
		timeCount += GetFrameTime();
	}
};

template<typename T>
struct gotoNode
{
	std::vector<std::unique_ptr<T>>* vec;
	size_t index;
};

class SceneBase
{
public:
	virtual gotoNode<SceneBase> update() = 0;
};

class MainScene : public SceneBase
{
private:
	ecs::World2D* world;
	bool isInitialized;
	gotoNode<SceneBase> selfNode;
	gotoNode<SceneBase> nextNode;
	std::string text0;
	std::string text1;
	std::string font;

	int textId;

	bool skip = false;

public:
	MainScene(ecs::World2D* world, gotoNode<SceneBase> selfNode, gotoNode<SceneBase> nextNode, std::string text0, std::string text1, std::string font) : world(world),selfNode(selfNode), nextNode(nextNode), isInitialized(false), text0(text0), text1(text1), font(font), textId(0) {}

	gotoNode<SceneBase> update() override
	{
		if (isInitialized)
		{
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyPressed(KEY_SPACE))
			{
				auto active = world->getDoubleBuffer<visualnovel::StandardTextBox>()->active()->get(textId);
				//auto inActive = world->getDoubleBuffer<galgame::StandardTextBox>()->active()->get(textId);
				if (active->activePixel >= active->totalPixel)
				{
					world->deleteUnit(textId);
					world->getEntityManager()->recycleId(textId);
					isInitialized = false;
					return nextNode;
				}
				else
				{
					world->getSystem<visualnovel::StandardTextBoxSystem>()->skip(textId);
					return selfNode;
				}
			}
			else
			{
				return selfNode;
			}
		}
		else
		{
			textId = world->getEntityManager()->getId();
			world->createUnit(textId, visualnovel::StandardTextBox(
				text0,
				text1,
				40,
				FontData,
				1.0f,
				{ float(GetScreenWidth() / 6), float(GetScreenHeight() / 3 * 2) },
				GetScreenWidth() / 3 * 2,
				{ 255, 255, 255, 255 }
			));
			isInitialized = true;
			return selfNode;
		}
	}
};

class SelectScene : public SceneBase
{
private:
	ecs::World2D* world;
	gotoNode<SceneBase> selfNode;
	gotoNode<SceneBase> nextNode0;
	gotoNode<SceneBase> nextNode1;

	std::string font;

	bool isInitialized;

	int b0Id;
	int b1Id;

public:
	SelectScene(ecs::World2D* world, gotoNode<SceneBase> selfNode, gotoNode<SceneBase> nextNode0, gotoNode<SceneBase> nextNode1, std::string font)
		: world(world), selfNode(selfNode), nextNode0(nextNode0), nextNode1(nextNode1), font(font), isInitialized(false), b0Id(0), b1Id(0) {}

	gotoNode<SceneBase> update() override
	{
		if (isInitialized)
		{
			auto b0 = world->getMessageManager()->getMessageList(b0Id);
			if (b0 != nullptr)
			{
				if (b0->size() > 0)
				{
					world->deleteUnit(b0Id);
					world->deleteUnit(b1Id);
					world->getEntityManager()->recycleId(b0Id);
					world->getEntityManager()->recycleId(b1Id);
					isInitialized = false;
					return nextNode0;
				}
			}
			auto b1 = world->getMessageManager()->getMessageList(b1Id);
			if (b1 != nullptr)
			{
				if (b1->size() > 0)
				{
					world->deleteUnit(b0Id);
					world->deleteUnit(b1Id);
					world->getEntityManager()->recycleId(b0Id);
					world->getEntityManager()->recycleId(b1Id);
					isInitialized = false;
					return nextNode1;
				}
			}
			return selfNode;
		}
		else
		{
			b0Id = world->getEntityManager()->getId();
			b1Id = world->getEntityManager()->getId();
			world->createUnit(b0Id, ui::ButtonExCom
			(
				FontData,//FONT_PATH,
				"resource\\img\\按钮0.png",
				"resource\\img\\按钮1.png",
				"resource\\img\\按钮2.png",
				u8"做雪饼",
				WHITE,
				50,
				5,
				{ 810,320 },
				{ 300,80 },
				8,
				1.0f
			));
			world->createUnit(b1Id, ui::ButtonExCom
			(
				FontData,
				"resource\\img\\按钮0.png",
				"resource\\img\\按钮1.png",
				"resource\\img\\按钮2.png",
				u8"做泡芙",
				WHITE,
				50,
				5,
				{ 810,680 },
				{ 300,80 },
				8,
				1.0f
			));
			//world->createUnit(b1Id, ui::ButtonExCom({ 810, 680 }, 300, 80, 50, WHITE, BLUE, u8"做泡芙", font));
			world->getSystem<ui::ButtonSystem>()->registerListener(b0Id, b0Id);
			world->getSystem<ui::ButtonSystem>()->registerListener(b1Id, b1Id);
			isInitialized = true;
			return selfNode;
		}
	}
};

class FlashScene : public SceneBase
{
private:
	ecs::World2D* world;
	bool isInitialized;
	gotoNode<SceneBase> selfNode;
	gotoNode<SceneBase> nextNode;
	std::string text0;
	std::string text1;
	std::string font;

	int textId;

	float timeCount = 0.0f;


	class Dr : public ecs::DrawBase
	{
	private:
		unsigned char a;
	public:
		Dr(unsigned char a) : a(a) {}

		void draw() override
		{
			DrawRectangle(0, 0, WinWidth, WinHeight, { 255,255,255,a });
		}
	};


public:
	FlashScene(ecs::World2D* world, gotoNode<SceneBase> selfNode, gotoNode<SceneBase> nextNode, std::string text0, std::string text1, std::string font) : world(world), selfNode(selfNode), nextNode(nextNode), isInitialized(false), text0(text0), text1(text1), font(font), textId(0) {}

	gotoNode<SceneBase> update() override
	{
		if (isInitialized)
		{
			if (timeCount < 1.0f)
			{
				timeCount += GetFrameTime();
				if (timeCount < 0.1f)
				{
					(*world->getUiLayer())[15].push_back(std::make_unique<Dr>(Dr(std::clamp(unsigned char(255.0f - (255.0f * (timeCount / 0.1f))), unsigned char(0), unsigned char(255)))));
				}
				else if (timeCount < 0.2f)
				{
					(*world->getUiLayer())[15].push_back(std::make_unique<Dr>(Dr(std::clamp(unsigned char(255.0f - (255.0f * ((timeCount - 0.1f) / 0.1f))), unsigned char(0), unsigned char(255)))));
				}
				else
				{
					(*world->getUiLayer())[15].push_back(std::make_unique<Dr>(Dr(std::clamp(unsigned char(255.0f - (255.0f * ((timeCount - 0.2f) / 1.0f))), unsigned char(0), unsigned char(255)))));
				}
				
				return selfNode;
			}
			else
			{
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyPressed(KEY_SPACE))
				{
					world->deleteUnit(textId);
					world->getEntityManager()->recycleId(textId);
					isInitialized = false;
					timeCount = 0.0f;
					return nextNode;
				}
				else
				{
					return selfNode;
				}
			}
		}
		else
		{
			textId = world->getEntityManager()->getId();
			world->createUnit(textId, visualnovel::StandardTextBox(
				text0,//u8"一段非常之长的，可用于测试自动换行的，包含符号的，没有任何现实意义与象征意义的，随便乱打的，废话连篇的测试文本",
				text1,//u8"非常に長い一段で、自動改行のテストに使用できる、意味や象徴的な意味のない、無作為に打たれた、冗長なテストテキストです。",
				40,
				FontData,
				1.0f,
				{ float(GetScreenWidth() / 6), float(GetScreenHeight() / 3 * 2) },
				GetScreenWidth() / 3 * 2,
				{ 255, 255, 255, 255 }
			));
			isInitialized = true;
			return selfNode;
		}
	}
};

class Main : public ecs::World2D
{
private:
	std::string font;

	std::vector<std::unique_ptr<SceneBase>> mainList;

	float& volume;
	

	class System : public ecs::SystemBase
	{
	private:
		std::vector<std::unique_ptr<SceneBase>>& mainList;
		gotoNode<SceneBase> pointer;
		bool active = false;
		float& volume;
		rlRAII::MusicRAII music = rlRAII::MusicRAII("resource\\music\\2.mp3");

	public:
		System(std::vector<std::unique_ptr<SceneBase>>& mainList, float& volume) : mainList(mainList), volume(volume), pointer({})
		{
			PlayMusicStream(music.get());
		}

		void update() override
		{
			SetMusicVolume(music.get(), volume);
			UpdateMusicStream(music.get());
			if (active)
			{
				gotoNode<SceneBase> pointerTemp;
				pointerTemp = (*pointer.vec)[pointer.index]->update();
				pointer = pointerTemp;
			}
			else
			{
				pointer = mainList[0]->update();
				active = true;
			}
		}
	};

public:
	Main(std::string font, int scrX, int scrY, float& volume) : font(font), World2D(scrX, scrY), volume(volume)
	{
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 0 }, { &mainList, 1 }, u8"你好。", u8"こんにちは。", font)));
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 1 }, { &mainList, 2 }, u8"这里并没有什么Galgame。", u8"ここには特にガルゲームはありません。", font)));
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 2 }, { &mainList, 3 }, u8"这只是一个平平无奇的测试。", u8"これはただの普通のテストです。", font)));
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 3 }, { &mainList, 4 }, u8"之后就是测试环节，你可以按ESC退出。", u8"その後はテストの段階です。ESCを押すと退出できます。", font)));
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 4 }, { &mainList, 5 }, u8"一段非常之长的，可用于测试自动换行的，包含符号的，没有任何现实意义与象征意义的，随便乱打的，废话连篇的测试文本。", u8"非常に長い一段で、自動改行のテストに使用できる、意味や象徴的な意味のない、無作為に打たれた、冗長なテストテキストです。", font)));
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 5 }, { &mainList, 6 }, u8"人们常说，路不止一条，看你怎么选择，即将进入选择测试。", u8"人々はよく言います、道は一つだけではなく、あなたがどう選ぶかによります。選択テストに入ろうとしています。", font)));
		mainList.push_back(std::make_unique<SelectScene>(SelectScene(this, { &mainList, 6 }, { &mainList, 7 }, { &mainList, 8 }, font)));

		//选择枝 1
		mainList.push_back(std::make_unique<FlashScene>(FlashScene(this, { &mainList, 7 }, { &mainList, 10 }, u8"你制作了雪饼。", u8"あなたは雪饼を作りました。", font)));

		//选择枝 2
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 8 }, { &mainList, 9 }, u8"在进行可能产生孩子的行为前，请填写C-1表格。", u8"子供を作る可能性のある行為を行う前に、C-1フォームに記入してください。", font)));
		mainList.push_back(std::make_unique<FlashScene>(FlashScene(this, { &mainList, 9 }, { &mainList, 10 }, u8"你制作了泡芙。", u8"あなたはシュークリームを作りました。", font)));

		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 10 }, { &mainList, 11 }, u8"测试完毕。", u8"テストが完了しました。", font)));
		mainList.push_back(std::make_unique<MainScene>(MainScene(this, { &mainList, 11 }, { &mainList, 0 }, u8"再按一下就会进入循环（因为懒得做终止）。", u8"もう一度押すとループに入ります（終わらせるのが面倒だから）。", font)));
		ui::ApplyTextBox(*this);
		ui::ApplyTextBoxEx(*this);
		ui::ApplyButtonEx(*this);
		ui::ApplyButton(*this);
		visualnovel::ApplyStandardTextBox(*this, CFG);

		this->addSystem(System(mainList, volume));
	}

};



int main0()
{
	CFG.ScreenWidth = 1920;
	CFG.ScreenHeight = 1080;
	CFG.textSpeed = 1.0f;


	SetConfigFlags(FLAG_WINDOW_UNDECORATED);// | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(1920, 1080, "");
	SetWindowPosition(0, 0);
	InitAudioDevice();
	
	SetExitKey(KEY_NULL);

		
	AllStates state = AllStates::Menu;

	bool showFPS = true;
	float volume = 1.0f;

	MenuWorld menu(state, WinWidth, WinHeight, volume);
	Config config(state, showFPS, volume, WinWidth, WinHeight);
	Main main(FONT_PATH, WinWidth, WinHeight, volume);

	while (!WindowShouldClose())
	{
		AllStates stateTmp = state;
		switch (stateTmp)
		{
		case AllStates::Menu:
			menu.update();
			break;

		case AllStates::Config:
			config.init(menu.getSceenshot().get().texture);
			config.update();
			break;

		case AllStates::Main:
			main.update();
			break;

		default:
			break;
		}
		if (showFPS)
		{
			DrawFPS(10, 40);
		}

		BeginDrawing();
		ClearBackground(BLACK);

		switch (stateTmp)
		{
		case AllStates::Menu:
			menu.draw();
			break;

		case AllStates::Config:
			config.draw();
			break;

		case AllStates::Main:
			main.draw();
			break;

		default:
			break;
		}
		if (showFPS)
		{
			DrawFPS(10, 40);
		}

		//DrawTextEx(f.get(), u8"一段非常之长的，可用于测试自动换行的，包含符号的，没有任何现实意义与象征意义的，随便乱打的，废话连篇的测试文本", { 20, 20 }, 50, 5, WHITE);
		EndDrawing();


		if (IsKeyPressed(KEY_ESCAPE))
		{
			switch (state)
			{
			case AllStates::Menu:
				CloseWindow();
				break;

			case AllStates::Config:
				state = AllStates::Menu;
				break;

			case AllStates::Main:
				state = AllStates::Menu;
				break;

			default:
				break;
			}
		}
	}
	return 0;
}
*/
#include "ScriptLoader.h"

int main()
{
	SetConfigFlags(FLAG_WINDOW_UNDECORATED);// | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(GetScreenWidth(), GetScreenHeight(), "");
	SetWindowPosition(0, 0);
	InitAudioDevice();
	InitTouchEx();

	SetExitKey(KEY_ESCAPE);

	CFG.textSpeed = 1.0f;
	CFG.fontData = rlRAII::FileRAII(FONT_PATH);
	CFG.textBoxBackGround = rlRAII::Texture2DRAII("resource\\img\\TextBoxBackground.png");
	CFG.chrNameBackGround = rlRAII::Texture2DRAII("resource\\img\\ChrBoxBackground.png");
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

	rlRAII::FileRAII sc("Script.txt");
	vn::ScriptLoader scLoader(main, "Script.txt", CFG);
	auto task = scLoader.load();
	vn::ApplyScriptLoader(main, scLoader, CFG);
	
	task.get();
	scLoader.loadScene(sc.begin());

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