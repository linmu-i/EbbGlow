#include <new>
#include <filesystem>
#include <string>

#include <raylib.h>
#include <EbbGlow/Resources/SharedMusic.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
	SharedMusic::SharedMusic() noexcept : data(nullptr) {}

	SharedMusic::SharedMusic(const SharedMusic& other) noexcept : data(other.data)
	{
		if (data != nullptr)
		{
			++(static_cast<MusicBlock*>(data)->refCnt);
		}
	}

	SharedMusic::SharedMusic(SharedMusic&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedMusic::SharedMusic(const std::filesystem::path& musicPath) noexcept
	{
		std::string pathStr = musicPath.string();
		std::filesystem::path path(musicPath);
		if (path != musicPath)//有损转换，改为文件读取
		{
			*this = SharedMusic(SharedFile(musicPath));
			return;
		}

		data = new(std::nothrow) MusicBlock(1, LoadMusicStream(pathStr.c_str()));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsMusicValid(static_cast<MusicBlock*>(data)->music))
		{
			delete static_cast<MusicBlock*>(data);
			data = nullptr;
			return;
		}
	}

	SharedMusic::SharedMusic(const SharedFile& fileData) noexcept : data(nullptr)
	{
		if (!fileData.valid())
		{
			return;
		}
		data = new(std::nothrow) MusicBlock(1, ::LoadMusicStreamFromMemory(fileData.fileExtension().string().c_str(), fileData.get(), fileData.size()), fileData);
		if (data == nullptr)
		{
			return;
		}
		if (!::IsMusicValid(static_cast<MusicBlock*>(data)->music))
		{
			delete static_cast<MusicBlock*>(data);
			data = nullptr;
			return;
		}
	}

	SharedMusic& SharedMusic::operator=(const SharedMusic& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		if (other.data != nullptr)
		{
			data = other.data;
			++(static_cast<MusicBlock*>(data)->refCnt);
		}
		return *this;
	}
	
	SharedMusic& SharedMusic::operator=(SharedMusic&& other) noexcept
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

	SharedMusic::~SharedMusic() noexcept
	{
		release();
	}

	void SharedMusic::release() noexcept
	{
		if (data != nullptr)
		{
			--(static_cast<MusicBlock*>(data)->refCnt);
			if (static_cast<MusicBlock*>(data)->refCnt == 0)
			{
				UnloadMusicStream(static_cast<MusicBlock*>(data)->music);
				delete static_cast<MusicBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedMusic::get() noexcept
	{
		if (data) return &(static_cast<MusicBlock*>(data)->music);
		return nullptr;
	}

	const void* SharedMusic::get() const noexcept
	{
		if (data) return &(static_cast<MusicBlock*>(data)->music);
		return nullptr;
	}
}
