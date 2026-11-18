#include <new>

#include <raylib.h>
#include <EbbGlow/Resources/SharedTexture.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
	SharedTexture::SharedTexture() noexcept : data(nullptr) {}

	SharedTexture::SharedTexture(const std::filesystem::path& path) noexcept : data(nullptr)
	{
		data = new(std::nothrow) TextureBlock(1, ::LoadTexture(reinterpret_cast<const char*>(path.u8string().c_str())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsTextureValid(((TextureBlock*)data)->texture))
		{
			delete (TextureBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedTexture::SharedTexture(const SharedImage& image) noexcept : data(nullptr)
	{
		if (!image.valid())
		{
			return;
		}
		data = new(std::nothrow) TextureBlock(1, ::LoadTextureFromImage(*reinterpret_cast<const ::Image*>(image.get())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsTextureValid(((TextureBlock*)data)->texture))
		{
			delete (TextureBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedTexture::SharedTexture(const SharedTexture& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<TextureBlock*>(data)->refCnt);
		}
	}

	SharedTexture::SharedTexture(SharedTexture&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedTexture::~SharedTexture() noexcept
	{
		release();
	}

	SharedTexture& SharedTexture::operator=(const SharedTexture& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(static_cast<TextureBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedTexture& SharedTexture::operator=(SharedTexture&& other) noexcept
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

	int SharedTexture::width() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<TextureBlock*>(data)->texture.width;
	}

	int SharedTexture::height() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<TextureBlock*>(data)->texture.height;
	}

	Vec2 SharedTexture::size() const noexcept
	{
		if (data == nullptr) return Vec2{ 0.0f, 0.0f };
		return Vec2{ static_cast<float>(width()), static_cast<float>(height())};
	}

	void SharedTexture::release() noexcept
	{
		if (data)
		{
			--(((TextureBlock*)data)->refCnt);
			if (((TextureBlock*)data)->refCnt == 0)
			{
				UnloadTexture(((TextureBlock*)data)->texture);
				delete (TextureBlock*)data;
			}
			data = nullptr;
		}
	}

	void* SharedTexture::get() noexcept
	{
		if (data) return &(static_cast<TextureBlock*>(data)->texture);
		return nullptr;
	}

	const void* SharedTexture::get() const noexcept
	{
		if (data) return &(static_cast<TextureBlock*>(data)->texture);
		return nullptr;
	}
}
