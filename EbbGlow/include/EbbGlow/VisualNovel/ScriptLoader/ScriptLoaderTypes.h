#include <string>
#include <vector>

namespace ebbglow::visualnovel
{
	struct Command
	{
		std::string name;
		std::vector<std::string> args;

		Command() = default;
		Command(const std::string& name, const std::vector<std::string>& args) : name(name), args(args) {}
		Command(std::string&& name, std::vector<std::string>&& args) : name(std::move(name)), args(std::move(args)) {}
	};

	struct SceneInfo
	{
		std::string name;
		std::string type;
		std::vector<std::string> args;

		SceneInfo() = default;
		SceneInfo(const std::string& name, const std::string& type, const std::vector<std::string>& args) : name(name), type(type), args(args) {}
		SceneInfo(std::string&& name, std::string&& type, std::vector<std::string>&& args) : name(std::move(name)), type(std::move(type)), args(std::move(args)) {}
	};

	struct VariableView
	{
		size_t index;
		size_t length;
	};

	struct MacroView
	{
		std::vector<std::string> args;
		std::string body;
	};

	struct LogView
	{
		std::string sceneName;
		std::string text;
		std::string exText;
		std::string voice;
		std::string music;
		void clear()
		{
			sceneName.clear();
			text.clear();
			exText.clear();
			voice.clear();
			music.clear();
		}
	};
}