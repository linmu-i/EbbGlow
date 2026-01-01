#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	void SkipSpace(rsc::SharedFile::Iterator& ptr) { while (!ptr.eof() && std::isspace(*ptr)) ++ptr; }
	double ParsePrimary(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader)
	{
		SkipSpace(ptr);
		if (ptr.eof() || *ptr == stop) return 0.0;

		char c = *ptr;

		if (c == '$')
		{
			++ptr;
			std::string varName;
			while (!ptr.eof() && *ptr != stop && (std::isalpha(static_cast<unsigned char>(*ptr)) || *ptr == '_' || isdigit(*ptr)))
			{
				varName += *ptr;
				++ptr;
			}

			int32_t index = 0;
			if (!ptr.eof() && *ptr != stop && *ptr == '[')
			{
				++ptr;
				SkipSpace(ptr);
				if (ptr.eof() || *ptr == stop) return 0.0;
				index = static_cast<int32_t>(round(ParseExpression(ptr, ']', scLoader)));

				SkipSpace(ptr);
				if (!ptr.eof() && *ptr != stop && *ptr == ']')
					++ptr;
			}
			auto varViewIt = scLoader.numberView.find(varName);
			if (varViewIt == scLoader.numberView.end()) return 0.0;

			index += varViewIt->second.index;
			if (index < 0 || index >= scLoader.numberStorage.size()) return 0.0;
			return scLoader.numberStorage[index];
		}

		if (c == '+')
		{
			++ptr;
			return +ParsePrimary(ptr, stop, scLoader);
		}
		if (c == '-')
		{
			++ptr;
			return -ParsePrimary(ptr, stop, scLoader);
		}

		if (std::isdigit(static_cast<unsigned char>(c)) || c == '.')
		{
			std::string numberBuf;
			while (!ptr.eof() && *ptr != stop &&
				(std::isdigit(static_cast<unsigned char>(*ptr)) || *ptr == '.'))
			{
				if (*ptr == '.' && numberBuf.find('.') != std::string::npos) break;
				numberBuf += *ptr;
				++ptr;
			}
			if (numberBuf.empty()) return 0.0;
			return std::stod(numberBuf);
		}

		if (c == '(')
		{
			++ptr;
			double val = ParseExpression(ptr, stop, scLoader);
			SkipSpace(ptr);
			if (!ptr.eof() && *ptr != stop && *ptr == ')')
				++ptr;
			return val;
		}

		return 0.0;
	}
	double ParseTerm(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader)
	{
		double result = ParsePrimary(ptr, stop, scLoader);
		SkipSpace(ptr);

		while (!ptr.eof() && *ptr != stop)
		{
			char op = *ptr;
			if (op == '*' || op == '/' || op == '%')
			{
				++ptr;
				SkipSpace(ptr);
				double rhs = (ptr.eof() || *ptr == stop) ? 0.0 : ParsePrimary(ptr, stop, scLoader);
				if (op == '*')
					result *= rhs;
				else if (op == '/')
					result = (rhs == 0.0) ? std::numeric_limits<double>::quiet_NaN() : result / rhs;
				else if (op == '%')
					result = std::fmod(result, rhs);
				SkipSpace(ptr);
			}
			else
				break;
		}
		return result;
	}
	double ParseExpression(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader)
	{
		double result = ParseTerm(ptr, stop, scLoader);
		SkipSpace(ptr);

		while (!ptr.eof() && *ptr != stop)
		{
			char op = *ptr;
			if (op == '+' || op == '-')
			{
				++ptr;
				SkipSpace(ptr);
				double rhs = (ptr.eof() || *ptr == stop) ? 0.0 : ParseTerm(ptr, stop, scLoader);
				if (op == '+')
					result += rhs;
				else
					result -= rhs;
				SkipSpace(ptr);
			}
			else
				break;
		}
		return result;
	}
	
	std::string GetStateTag(std::string_view token)
	{
		return (token.empty() || token[0] != '@') ? "" : std::string(token.substr(1));
	}
	std::string GetNextString(rsc::SharedFile::Iterator& it, ScriptLoader& scLoader)
	{
		std::string textBuf;
		while (*it != '"' && *it != '$' && !it.eof()) ++it;
		if (*it == '$')
		{
			++it;
			std::string varName;
			while (!isspace(*it) && *it != '[' && !it.eof())
			{
				varName += *it;
				++it;
			}
			int32_t index = 0;
			if (*it == '[')
			{
				++it;
				std::string indexBuf;
				while (*it != ']' && !it.eof())
				{
					indexBuf += *it;
					++it;
				}
				if (!indexBuf.empty())
					index = static_cast<int32_t>(std::stoi(indexBuf));
				if (*it == ']')
					++it;
			}
			auto varViewIt = scLoader.textView.find(varName);
			if (varViewIt == scLoader.textView.end()) return "";
			index += varViewIt->second.index;
			if (index < 0 || index >= scLoader.textStorage.size()) return "";
			return scLoader.textStorage[index];
		}
		++it;
		while (*it != '"' && !it.eof())
		{
			if (*it == '\\' && it.position() < it.size() - 1)
			{
				textBuf += it[1];
				it += 2;
			}
			else
			{
				textBuf += *it;
				++it;
			}
		}
		++it;
		return textBuf;
	};
	std::string GetString(std::string_view token, ScriptLoader& scLoader)
	{
		auto it = rsc::SharedFile::Iterator(token.size(), reinterpret_cast<unsigned char*>(const_cast<char*>(token.data())), 0);
		std::string result;
		while (!it.eof())
		{
			if (*it == '+') ++it;
			result += GetNextString(it, scLoader);
		}
		++it;
		return result;
	}
	double GetNumber(std::string_view token, unsigned char stop, ScriptLoader& scLoader)
	{
		auto it = rsc::SharedFile::Iterator(token.size(), reinterpret_cast<unsigned char*>(const_cast<char*>(token.data())), 0);
		double result = ParseExpression(it, stop, scLoader);
		while ((*it != ',' && *it != ')') && !it.eof()) ++it;
		++it;
		return result;
	}

	std::string* GetTextVariable(const std::string& name, ScriptLoader& scLoader) noexcept
	{
		auto varViewIt = scLoader.textView.find(name);
		if (varViewIt == scLoader.textView.end()) return nullptr;
		int32_t index = varViewIt->second.index;
		if (index < 0 || index >= scLoader.textStorage.size()) return nullptr;
		return &scLoader.textStorage[index];
	}
	double* GetNumberVariable(const std::string& name, ScriptLoader& scLoader) noexcept
	{
		auto varViewIt = scLoader.numberView.find(name);
		if (varViewIt == scLoader.numberView.end()) return nullptr;
		int32_t index = varViewIt->second.index;
		if (index < 0 || index >= scLoader.numberStorage.size()) return nullptr;
		return &scLoader.numberStorage[index];
	}

	std::future<void> ScriptLoader::init(const std::string& filePath) noexcept
	{
		return std::async(std::launch::async, [this, filePath]() noexcept
			{
				scriptData = ScriptData(filePath);
				scIt = scriptData.begin();
				while (!scIt.eof())
				{
					if (!memcmp(scIt.get(), "Global", 6) && isspace(scIt[6]))
					{
						scIt += 6;
						SkipSpace(scIt);
						while (!scIt.eof())
						{
							if ((!memcmp(scIt.get(), "Scene", 5) && isspace(scIt[5])) || (!memcmp(scIt.get(), "Global", 6) && isspace(scIt[6])))
							{
								break;
							}
							if (!memcmp(scIt.get(), "Macro", 5) && isspace(scIt[5]))
							{
								scIt += 5;
								SkipSpace(scIt);
								auto args = Tokenizer(scIt);

								MacroView mv;
								mv.args = args.args;
								SkipSpace(scIt);
								++scIt;

								uint8_t count = 1;
								while (count && !scIt.eof())
								{
									switch (*scIt)
									{
									case '{':
										mv.body += '{';
										++count;
										break;

									case '}':
										--count;
										if (count) mv.body += '}';
										break;

									case '"':
										mv.body += '"';
										++scIt;
										while (*scIt != '"' && !scIt.eof())
										{
											if (*scIt == '\\')
											{
												mv.body += '\\';
												++scIt;
												mv.body += *scIt;
											}
											else
											{
												mv.body += *scIt;
											}
											++scIt;
										}
										mv.body += '"';
										break;
									default:
										mv.body += *scIt;
									}
									++scIt;
								}
								macroView.emplace(std::move(args.name), std::move(mv));
							}
							else if (isalpha(*scIt))
							{
								auto cmd = Tokenizer(scIt);
								Invoker(cmd, globalFunctions);
							}
							else
							{
								++scIt;
							}
						}

					}
					else if (!memcmp(scIt.get(), "Scene", 5) && isspace(scIt[5]))
					{
						int64_t offset = scIt.position();
						scIt += 5;
						std::string scName;
						SkipSpace(scIt);
						while (*scIt != '(' && !scIt.eof() && !isspace(*scIt))
						{
							scName += *scIt;
							++scIt;
						}
						sceneView.emplace(std::move(scName), offset);

						while (!scIt.eof())//跳过所有字符直到下一段
						{
							if (*scIt == 'G' || *scIt == 'S')
							{
								if (((!memcmp(scIt.get(), "Global", 6) && isspace(scIt[6])) || (!memcmp(scIt.get(), "Scene", 5)) && isspace(scIt[5])) && (scIt.position() == 0 || scIt[-1] == '\n'))
								{
									break;
								}
							}
							else if (*scIt == '"' && !scIt.eof())
							{
								++scIt;
								while (*scIt != '"' && !scIt.eof())
								{
									if (*scIt == '\\') ++scIt;
									++scIt;
								}
							}
							++scIt;
						}
					}
					else
					{
						++scIt;
					}
				}
			});
	}

	Command ScriptLoader::Tokenizer(rsc::SharedFile::Iterator& it) noexcept
	{
		SkipSpace(it);
		std::string funcName;
		for (; *it != '(' && !isspace(*it) && !it.eof(); ++it) funcName += *it;
		SkipSpace(it);
		++it;
		SkipSpace(it);
		if (*it == ')') return Command(std::move(funcName), std::move(std::vector<std::string>()));
		std::vector<std::string> args;
		args.push_back("");
		while (*it != ')' && !it.eof())
		{
			switch (*it)
			{
			case '/':
				if (it[1] == '/')
				{
					while (*it != '\n' && !it.eof()) ++it;
				}
				else if (it[1] == '*')
				{
					it += 2;
					while (!it.eof() && !(*it == '/' && it[-1] == '*')) ++it;
				}
				else
				{
					args.back() += *it;
				}
				++it;
				break;
			case ',':
				while (!args.back().empty() && isspace(args.back().back())) args.back().pop_back();
				args.push_back("");
				++it;
				SkipSpace(it);
				break;

			case '"':
				while (!it.eof() && *it != '"')
				{
					if (*it == '\\')
					{
						args.back() += *it;
						++it;
						if (it.eof()) break;
					}
					args.back() += *it;
					++it;
				}
				args.back() += '"';
				++it;
				break;
			default:
				args.back() += *it;
				++it;
			}
		}
		while (!it.eof() && (*it == ';' || *it == '\n')) ++it;
		++it;
		return Command(std::move(funcName), std::move(args));
	}

	void ScriptLoader::Invoker(const Command& cmd, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept
	{
		auto func = functions.find(cmd.name);
		if (func == functions.end()) return;
		(*func).second(this, cmd.args);
	}

	SceneInfo ScriptLoader::ReadSceneInfo(rsc::SharedFile::Iterator& it) noexcept
	{
		if (!(it.position() == 0 || isspace(it[-1])) || memcmp(it.get(), "Scene", 5) || !isspace(it[5])) return {};
		it += 5;
		SkipSpace(it);
		std::string sceneName;
		while (!it.eof() && *it != '(' && !isspace(*it))
		{
			sceneName += *it;
			++it;
		}
		while (!it.eof() && *it != '(') ++it;
		++it;
		SkipSpace(it);
		std::string sceneType;
		while (!it.eof() && !isspace(*it))
		{
			sceneType += *it;
			++it;
		}
		while (!it.eof() && *it != ':') ++it;
		++it;
		SkipSpace(it);
		if (*it == ')') return SceneInfo(std::move(sceneType), std::move(sceneName), std::move(std::vector<std::string>()));
		std::vector<std::string> args;
		args.push_back("");
		while (!it.eof() && *it != ')')
		{
			if (*it == ',')
			{
				while (!args.back().empty() && isspace(args.back().back())) args.back().pop_back();
				args.push_back("");
				++it;
				SkipSpace(it);
			}
			else
			{
				args.back() += *it;
				++it;
			}
		}
		while (!it.eof() && *it != '\n') ++it;
		++it;
		return SceneInfo(std::move(sceneName), std::move(sceneType), std::move(args));
	}

	bool IsKeyWord(rsc::SharedFile::Iterator& it, std::string_view keyWord)
	{
		return (!memcmp(it.get(), keyWord.data(), keyWord.size()) && (isspace(it[-1]) || it.position() == 0) && isspace(it[keyWord.size()]));
	}

	bool IsValidName(unsigned char chr)
	{
		return isdigit(chr) || isalpha(chr) || chr == '-' || chr == '_';
	}

	void ScriptLoader::ExecuteFunction(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept
	{
		while (!IsKeyWord(it, "Scene") && !IsKeyWord(it, "Global") && !it.eof())
		{
			switch (*it)
			{
			case '/':
				if (it[1] == '/')
				{
					while (*it != '\n' && !it.eof()) ++it;
				}
				else if (it[1] == '*')
				{
					it += 2;
					while (!it.eof() && !(*it == '/' && it[-1] == '*')) ++it;
				}
				++it;
				break;

			case '&':
				++it;
				ExecuteMacro(it, functions);
				break;

			case 'i':
				if (it[1] == 'f')
				{
					it += 2;
					while (*it != '(' && !it.eof()) ++it;
					++it;
					double var0 = ParseExpression(it, ')', *this);
					bool cond = false;
					SkipSpace(it);
					if (*it == '<')
					{
						if (it[1] == '=')
						{
							it += 2;
							cond = (var0 <= ParseExpression(it, ')', *this));
						}
						else
						{
							it += 1;
							cond = (var0 < ParseExpression(it, ')', *this));
						}
					}
					else if (*it == '>')
					{
						if (it[1] == '=')
						{
							it += 2;
							cond = (var0 >= ParseExpression(it, ')', *this));
						}
						else
						{
							it += 1;
							cond = (var0 > ParseExpression(it, ')', *this));
						}
					}
					else if (*it == '=')
					{
						if (it[1] == '=')
						{
							it += 2;
							cond = (var0 == ParseExpression(it, ')', *this));
						}
					}
					else if (*it == '!')
					{
						if (it[1] == '=')
						{
							it += 2;
							cond = (var0 != ParseExpression(it, ')', *this));
						}
					}
					else
					{
						cond = var0 != 0.0;
					}
					if (cond)
					{
						while (*it != '{' && !it.eof()) ++it;
						++it;
					}
					else
					{
						while (*it != '{' && !it.eof()) ++it;
						++it;
						int braceCount = 1;
						while (!it.eof() && braceCount > 0)
						{
							if (*it == '{') ++braceCount;
							else if (*it == '}') --braceCount;
							else if (*it == '"')
							{
								++it;
								while (*it != '"' && !it.eof())
								{
									if (*it == '\\') ++it;
									++it;
								}
							}
							else if (*it == '/')
							{
								if (it[1] == '/')
								{
									while (*it != '\n' && !it.eof()) ++it;
								}
								else if (it[1] == '*')
								{
									it += 2;
									while (!it.eof() && !(*it == '/' && it[-1] == '*')) ++it;
								}
							}
							++it;
						}
					}
				}
				break;
			default:
				if (isalpha(*it) || *it == '_')
				{
					auto cmd = Tokenizer(it);
					Invoker(cmd, functions);
				}
				else
				{
					++it;
				}
			}
			SkipSpace(it);
		}
	}

	void ScriptLoader::ExecuteMacro(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, std::vector<std::string>)>>& functions) noexcept
	{
		auto macroCmd = Tokenizer(it);
		auto viewIt = macroView.find(macroCmd.name);
		if (viewIt == macroView.end()) return;
		auto& view = viewIt->second;
		std::string body;
		body.reserve(view.body.size());
		for (int i = 0; i < view.body.size(); ++i)
		{
			if (view.body[i] == '$')
			{
				++i;
				std::string argName;
				while (!isspace(view.body[i]) && view.body[i] != ',' && view.body[i] != ')' && view.body[i] != '$' && i < view.body.size())
				{
					argName += view.body[i];
					++i;
				}
				size_t index = 0;
				for (; index < view.args.size(); ++index)
				{
					if (view.args[index] == argName) break;
				}
				if (index >= macroCmd.args.size())
				{
					body += "";
				}
				else
				{
					body += macroCmd.args[index];
				}
				body += view.body[i];
			}
			else
			{
				body += view.body[i];
			}
		}
		rsc::SharedFile::Iterator newIt(body.size(), reinterpret_cast<unsigned char*>(const_cast<char*>(body.c_str())));
		ExecuteFunction(newIt, functions);
	}

	void ScriptLoader::loadScene(rsc::SharedFile::Iterator& it) noexcept
	{
		if (!IsKeyWord(it, "Scene")) return;
		logTmp.clear();
		auto scInfo = ReadSceneInfo(it);
		sceneName = std::move(scInfo.name);
		sceneType = std::move(scInfo.type);
		sceneArgs = std::move(scInfo.args);
		SkipSpace(it);

		auto sceneTypeIt = sceneCreator.find(sceneType);
		if (sceneTypeIt == sceneCreator.end()) return;
		(*sceneTypeIt).second(this, sceneArgs);

		ExecuteFunction(it, sceneFunctions);
	}

	void ScriptLoader::registerSceneType(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept
	{
		sceneCreator.emplace(name, function);
	}
	void ScriptLoader::registerSceneFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept
	{
		sceneFunctions.emplace(name, function);
	}
	void ScriptLoader::registerGlobalFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function) noexcept
	{
		globalFunctions.emplace(name, function);
	}

	void ScriptLoader::addLog(const LogView& logView) noexcept
	{
		this->logView.push_back(logView);
	}
	void ScriptLoader::addLog(LogView&& logView) noexcept
	{
		this->logView.push_back(std::move(logView));
	}

	void ScriptLoader::start() noexcept
	{
		auto view = sceneView.find(beginScene);
		if (view == sceneView.end()) return;
		size_t index = view->second;
		auto beginIt = rsc::SharedFile::Iterator(scriptData.getSize(), scriptData.getData(), index);
		loadScene(beginIt);
	}
}