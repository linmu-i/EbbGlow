#include <new>

#include <raylib.h>
#include <EbbGlow/Resources/SharedSound.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
	SharedSound::SharedSound() noexcept : data(nullptr) {}

	SharedSound::SharedSound(const SharedSound& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<SoundBlock*>(data)->refCnt);
		}
	}

	SharedSound::SharedSound(SharedSound&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedSound::SharedSound(const std::filesystem::path& soundPath) noexcept
	{
		data = new(std::nothrow) SoundBlock(1, LoadSound(reinterpret_cast<const char*>(soundPath.u8string().c_str())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsSoundValid(static_cast<SoundBlock*>(data)->sound))
		{
			delete static_cast<SoundBlock*>(data);
			data = nullptr;
		}
	}

	SharedSound& SharedSound::operator=(const SharedSound& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(static_cast<SoundBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedSound& SharedSound::operator=(SharedSound&& other) noexcept
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

	SharedSound::~SharedSound() noexcept
	{
		release();
	}

	void SharedSound::release() noexcept
	{
		if (data)
		{
			--(static_cast<SoundBlock*>(data)->refCnt);
			if (static_cast<SoundBlock*>(data)->refCnt == 0)
			{
				UnloadSound(static_cast<SoundBlock*>(data)->sound);
				delete static_cast<SoundBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedSound::get() noexcept
	{
		if (data) return &(static_cast<SoundBlock*>(data)->sound);
		return nullptr;
	}

	const void* SharedSound::get() const noexcept
	{
		if (data) return &(static_cast<SoundBlock*>(data)->sound);
		return nullptr;
	}
}
