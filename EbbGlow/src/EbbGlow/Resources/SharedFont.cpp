#include <new>
#include <string>
#include <vector>

#include <raylib.h>
#include <EbbGlow/Resources/SharedFont.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
	SharedFont::SharedFont() noexcept : data(nullptr) {}

	SharedFont::SharedFont(const std::filesystem::path& path, float fontSize, std::vector<int32_t> codepoints) noexcept
	{
		if (codepoints.empty())
		{
			codepoints.reserve(98);
			for (int32_t i = 32; i < 127; ++i)
			{
				codepoints.push_back(i);
			}
		}
		codepoints.reserve(codepoints.size() + 3);
		codepoints.push_back('1');
		codepoints.push_back('1');
		codepoints.push_back('1');
		data = new(std::nothrow) FontBlock(1, LoadFontEx(reinterpret_cast<const char*>(path.u8string().c_str()), fontSize, codepoints.data(), codepoints.size()));
		if (!data) return;
		if (!::IsFontValid(static_cast<FontBlock*>(data)->font))
		{
			delete static_cast<FontBlock*>(data);
			data = nullptr;
		}
	}

	SharedFont::SharedFont(const SharedFile& fileData, float fontSize, std::vector<int32_t> codepoints) noexcept : data(nullptr)
	{
		if (!fileData.valid()) return;
		if (codepoints.empty())
		{
			codepoints.reserve(98);
			for (int32_t i = 32; i < 127; ++i)
			{
				codepoints.push_back(i);
			}
		}
		codepoints.reserve(codepoints.size() + 3);
		codepoints.push_back('1');
		codepoints.push_back('1');
		codepoints.push_back('1');
		data = new(std::nothrow) FontBlock(1, LoadFontFromMemory(fileData.fileExtension().string().c_str(), fileData.get(), fileData.size(), fontSize, codepoints.data(), codepoints.size()));
		if (!data) return;
		if (!::IsFontValid(static_cast<FontBlock*>(data)->font))
		{
			delete static_cast<FontBlock*>(data);
			data = nullptr;
		}
	}

	SharedFont::SharedFont(const SharedFont& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<FontBlock*>(data)->refCnt);
		}
	}

	SharedFont::SharedFont(SharedFont&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedFont::~SharedFont()
	{
		release();
	}

	SharedFont& SharedFont::operator=(const SharedFont& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(static_cast<FontBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedFont& SharedFont::operator=(SharedFont&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	void SharedFont::release() noexcept
	{
		if (data)
		{
			--(static_cast<FontBlock*>(data)->refCnt);
			if (static_cast<FontBlock*>(data)->refCnt == 0)
			{
				UnloadFont(static_cast<FontBlock*>(data)->font);
				delete static_cast<FontBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedFont::get() noexcept
	{
		if (data) return &(static_cast<FontBlock*>(data)->font);
		return nullptr;
	}

	const void* SharedFont::get() const noexcept
	{
		if (data) return &(static_cast<FontBlock*>(data)->font);
		return nullptr;
	}
}
