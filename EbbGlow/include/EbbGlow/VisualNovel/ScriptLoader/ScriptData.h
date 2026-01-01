#pragma once

#include <fstream>

#include <EbbGlow/Utils/Resource.h>

namespace ebbglow::visualnovel
{
	class ScriptData
	{
	private:
		unsigned char* data;
		size_t size;

	public:
		ScriptData() : data(nullptr), size(0) {}
		ScriptData(const std::string& filePath);
		ScriptData(const ScriptData&) = delete;
		ScriptData(ScriptData&& other) noexcept : data(other.data), size(other.size)
		{
			other.data = nullptr;
			other.size = 0;
		}
		~ScriptData() { delete[] data; }

		ScriptData& operator=(const ScriptData&) = delete;
		ScriptData& operator=(ScriptData&& other) noexcept;
		ScriptData& operator+=(const ScriptData& other);
		ScriptData& operator+=(const rsc::SharedFile& other);

		rsc::SharedFile::Iterator begin() { return rsc::SharedFile::Iterator(size, data); }
		rsc::SharedFile::Iterator end() { return rsc::SharedFile::Iterator(size, data, size); }

		const unsigned char* getData() const { return data; }
		unsigned char* getData() { return data; }
		size_t getSize() const { return size; }
	};
}