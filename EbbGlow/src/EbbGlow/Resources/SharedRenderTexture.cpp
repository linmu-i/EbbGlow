#include <new>

#include <raylib.h>
#include <EbbGlow/Resources/SharedRenderTexture.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
	SharedRenderTexture::SharedRenderTexture() noexcept : data(nullptr) {}

	SharedRenderTexture::SharedRenderTexture(const SharedRenderTexture& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<RenderTextureBlock*>(data)->refCnt);
		}
	}

	SharedRenderTexture::SharedRenderTexture(SharedRenderTexture&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedRenderTexture::SharedRenderTexture(int x, int y) noexcept
	{
		data = new(std::nothrow) RenderTextureBlock(1, LoadRenderTexture(x, y));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsRenderTextureValid(static_cast<RenderTextureBlock*>(data)->renderTexture))
		{
			delete static_cast<RenderTextureBlock*>(data);
			data = nullptr;
		}
	}

	SharedRenderTexture& SharedRenderTexture::operator=(const SharedRenderTexture& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data != nullptr)
		{
			++(static_cast<RenderTextureBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedRenderTexture& SharedRenderTexture::operator=(SharedRenderTexture&& other) noexcept
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

	SharedRenderTexture::~SharedRenderTexture()
	{
		release();
	}

	int SharedRenderTexture::width() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->renderTexture.texture.width;
		return 0;
	}

	int SharedRenderTexture::height() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->renderTexture.texture.height;
		return 0;
	}

	Vec2 SharedRenderTexture::size() const noexcept
	{
		return Vec2{ static_cast<float>(width()), static_cast<float>(height()) };
	}

	void SharedRenderTexture::release() noexcept
	{
		if (data)
		{
			--(static_cast<RenderTextureBlock*>(data)->refCnt);
			if (static_cast<RenderTextureBlock*>(data)->refCnt == 0)
			{
				RenderTexture& rt = static_cast<RenderTextureBlock*>(data)->renderTexture;
				rt.depth = {};
				rt.texture = {};
				::UnloadRenderTexture(rt);
				delete static_cast<RenderTextureBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedRenderTexture::get() noexcept
	{
		if (data) return &(static_cast<RenderTextureBlock*>(data)->renderTexture);
		return nullptr;
	}

	const void* SharedRenderTexture::get() const noexcept
	{
		if (data) return &(static_cast<RenderTextureBlock*>(data)->renderTexture);
		return nullptr;
	}

	SharedTexture& SharedRenderTexture::texture() noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->texture;
		static SharedTexture empty;
		return empty;
	}
	
	const SharedTexture& SharedRenderTexture::texture() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->texture;
		static SharedTexture empty;
		return empty;
	}

	SharedTexture& SharedRenderTexture::depth() noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->depth;
		static SharedTexture empty;
		return empty;
	}

	const SharedTexture& SharedRenderTexture::depth() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->depth;
		static SharedTexture empty;
		return empty;
	}
}
