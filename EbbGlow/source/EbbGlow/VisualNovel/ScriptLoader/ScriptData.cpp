#include <EbbGlow/VisualNovel/ScriptLoader/ScriptData.h>

namespace ebbglow::visualnovel
{
	ScriptData::ScriptData(const const std::string& filePath)
	{
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (file)
		{
			size = file.tellg();
			data = new unsigned char[size];
			file.seekg(0, std::ios::beg);
			file.read(reinterpret_cast<char*>(data), size);
			file.close();
		}
		else
		{
			data = nullptr;
			size = 0;
		}
	}

	ScriptData& ScriptData::operator=(ScriptData&& other) noexcept
	{
		if (this != &other)
		{
			delete[] data;
			data = other.data;
			size = other.size;
			other.data = nullptr;
			other.size = 0;
		}
		return *this;
	}

	ScriptData& ScriptData::operator+=(const ScriptData& other)
	{
		if (other.data)
		{
			unsigned char* newData = new unsigned char[size + other.size + 1];
			memcpy(newData, data, size);
			newData[size] = '\n';
			memcpy(newData + size + 1, other.data, other.size);
			delete[] data;
			data = newData;
			size += (other.size + 1);
		}
		return *this;
	}

	ScriptData& ScriptData::operator+=(const rsc::SharedFile& other)
	{
		unsigned char* newData = new unsigned char[size + other.size() + 1];
		memcpy(newData, data, size);
		newData[size] = '\n';
		memcpy(newData + size + 1, other.get(), other.size());
		delete[] data;
		data = newData;
		size += (other.size() + 1);
		return *this;
	}
}