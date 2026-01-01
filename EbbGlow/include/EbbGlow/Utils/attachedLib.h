#pragma once

#include <string>
inline uint64_t FNV_1a(const std::string& str)
{
	constexpr uint64_t offset_basis = 0xCBF29CE484222325ui64;
	constexpr uint64_t prime = 0x100000001B3ui64;

	uint64_t hash = offset_basis;
	for (auto it = str.begin(); it != str.end(); it++)
	{
		hash ^= static_cast<uint8_t>(*it);
		hash *= prime;
	}
	return hash;
}


inline int ExecuteBF(const char* cmds, size_t size, std::function<int(void)> input = getchar, std::function<void(int)> output = putchar)
{
	std::vector<char> memory;
	memory.resize(size, 0);
	const char* cmd = cmds;
	char* pointer = memory.data();
	int loopCount = 0;
	while (*cmd != '\0')
	{
		if (cmd < cmds || pointer < memory.data() || pointer >= memory.data() + size)
			return 0;
		if (loopCount == 0)
		{
			switch (*cmd)
			{
			case '>':
				++pointer;
				++cmd;
				break;
			case '<':
				--pointer;
				++cmd;
				break;
			case '+':
				++(*pointer);
				++cmd;
				break;
			case '-':
				--(*pointer);
				++cmd;
				break;
			case '.':
				output(*pointer);
				++cmd;
				break;
			case ',':
				*pointer = input();
				++cmd;
				break;
			case '[':
				if (!*pointer)
					loopCount = -1;
				else
					++cmd;
				break;
			case ']':
				if (*pointer)
					loopCount = 1;
				else
					++cmd;
				break;
			default:
				++cmd;
			}
		}
		else
		{
			if (loopCount < 0)
			{
				++cmd;
			}
			else
			{
				--cmd;
			}
			if (*cmd == ']')
				++loopCount;
			if (*cmd == '[')
				--loopCount;
		}
	}
	return (loopCount == 0 ? 1 : 0);
}