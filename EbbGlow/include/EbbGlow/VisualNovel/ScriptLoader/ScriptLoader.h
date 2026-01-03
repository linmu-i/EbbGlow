#pragma once

#include <string>

#include <EbbGlow/Core/World.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptData.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>
#include <EbbGlow/VisualNovel/ScriptLoader/MusicManager.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoaderTypes.h>
#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/UI/UI.h>

namespace ebbglow::visualnovel
{
	class ScriptLoader
	{
	private:
		//处理函数
		std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>> globalFunctions;
		std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>> sceneFunctions;
		std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>> sceneCreator;

		Command Tokenizer(rsc::SharedFile::Iterator& it) noexcept;
		void Invoker(const Command& cmd, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept;
		void ExecuteFunction(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept;
		void ExecuteMacro(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept;
		SceneInfo ReadSceneInfo(rsc::SharedFile::Iterator& it) noexcept;

	public:
		core::World2D& world;
		MusicManager& musicMgr;

		//变量存储
		std::vector<double> numberStorage;
		std::vector<std::string> textStorage;
		std::unordered_map<std::string, VariableView> numberView;
		std::unordered_map<std::string, VariableView> textView;

		//宏函数
		std::unordered_map<std::string, MacroView> macroView;

		//定位缓存
		std::unordered_map<std::string, int64_t> sceneView;

		//文本回看
		std::vector<LogView> logView;

		//初始化数据
		ScriptData scriptData;
		std::string beginScene;

		//运行时数据
		std::vector<core::entity> idList;
		std::vector<core::entity> exIdList;
		std::vector<std::string> sceneArgs;
		std::string sceneName;
		std::string sceneType;
		rsc::SharedFile::Iterator scIt;//解析时使用的迭代器，供追加文本时更新迭代器使用
		LogView logTmp;

		//设置
		VisualNovelConfig& cfg;

		ScriptLoader(core::World2D& world, VisualNovelConfig& cfg, MusicManager* musicMgr) : world(world), cfg(cfg), musicMgr(*musicMgr) {}
		std::future<void> init(const std::string& filePath) noexcept;
		void loadScene(rsc::SharedFile::Iterator& it) noexcept;
		void start() noexcept;
		void registerSceneType(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept;
		void registerSceneFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept;
		void registerGlobalFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept;
		void addLog(const LogView& logView) noexcept;
		void addLog(LogView&& logView) noexcept;
	};
	 
	void SkipSpace(rsc::SharedFile::Iterator& ptr);
	double ParsePrimary(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseTerm(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseExpression(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	std::string GetStateTag(std::string_view token);
	std::string GetNextString(rsc::SharedFile::Iterator& it, ScriptLoader& scLoader);
	std::string GetString(std::string_view token, ScriptLoader& scLoader);
	double GetNumber(std::string_view token, unsigned char stop, ScriptLoader& scLoader);
	std::string* GetTextVariable(const std::string& name, ScriptLoader& scLoader) noexcept;
	double* GetNumberVariable(const std::string& name, ScriptLoader& scLoader) noexcept;
}