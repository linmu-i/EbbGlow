#include <new>

#include <raylib.h>
#include <EbbGlow/Resources/SharedImage.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
	SharedImage::SharedImage() noexcept : data(nullptr) {}

	SharedImage::SharedImage(const std::filesystem::path& path) noexcept : data(nullptr)
	{
		data = new(std::nothrow) ImageBlock(1, ::LoadImage(reinterpret_cast<const char*>(path.u8string().c_str())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsImageValid(((ImageBlock*)data)->image))
		{
			delete (ImageBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedImage::SharedImage(const SharedFile& file) noexcept : data(nullptr)
	{
		if (!file.valid())
		{
			return;
		}
		data = new(std::nothrow) ImageBlock(1, ::LoadImageFromMemory(reinterpret_cast<const char*>(file.fileExtension().u8string().c_str()), file.get(), file.size()));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsImageValid(((ImageBlock*)data)->image))
		{
			delete (ImageBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedImage::SharedImage(const SharedImage& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(((ImageBlock*)data)->refCnt);
		}
	}

	SharedImage::SharedImage(SharedImage&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedImage::~SharedImage() noexcept
	{
		release();
	}

	SharedImage& SharedImage::operator=(const SharedImage& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(((ImageBlock*)data)->refCnt);
		}
		return *this;
	}

	SharedImage& SharedImage::operator=(SharedImage&& other) noexcept
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

	int SharedImage::width() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<ImageBlock*>(data)->image.width;
	}

	int SharedImage::height() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<ImageBlock*>(data)->image.height;
	}

	Vec2 SharedImage::size() const noexcept
	{
		return Vec2{ static_cast<float>(width()), static_cast<float>(height()) };
	}

	PixelFormat SharedImage::pixelFormat() const noexcept
	{
		if (data == nullptr) return PixelFormat::Other;
		switch (static_cast<ImageBlock*>(data)->image.format)
		{
		case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: return PixelFormat::R8G8B8A8;
		case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: return PixelFormat::GrayScale;
		default: return PixelFormat::Other;
		}
	}

	uint8_t* SharedImage::pixelData() noexcept
	{
		if (data == nullptr) return nullptr;
		return static_cast<uint8_t*>(static_cast<ImageBlock*>(data)->image.data);
	}

	const uint8_t* SharedImage::pixelData() const noexcept
	{
		if (data == nullptr) return nullptr;
		return static_cast<uint8_t*>(static_cast<ImageBlock*>(data)->image.data);
	}

	void* SharedImage::get() noexcept
	{
		if (data == nullptr) return nullptr;
		return &(static_cast<ImageBlock*>(data)->image);
	}

	const void* SharedImage::get() const noexcept
	{
		if (data == nullptr) return nullptr;
		return &(static_cast<ImageBlock*>(data)->image);
	}

	void SharedImage::release() noexcept
	{
		if (data)
		{
			--(((ImageBlock*)data)->refCnt);
			if (((ImageBlock*)data)->refCnt == 0)
			{
				UnloadImage(((ImageBlock*)data)->image);
				delete (ImageBlock*)data;
			}
			data = nullptr;
		}
	}
}
