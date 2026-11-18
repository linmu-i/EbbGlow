#include "ResourceCreator.h"
#include "ResourceBlock.h"

namespace ebbglow::resource
{
	SharedFont ResourceCreator::CreateFont(::Font font) noexcept
	{
		SharedFont result;
		if (!::IsFontValid(font)) return result;
		result.data = new(std::nothrow) FontBlock(1, font);
		if (!result.data) return result;
		if (!::IsFontValid(static_cast<FontBlock*>(result.data)->font))
		{
			delete static_cast<FontBlock*>(result.data);
			result.data = nullptr;
		}
		return result;
	};

	SharedTexture ResourceCreator::CreateTexture(::Texture2D texture) noexcept
	{
		SharedTexture result;
		if (!::IsTextureValid(texture)) return result;
		result.data = new(std::nothrow) TextureBlock(1, texture);
		if (!result.data) return result;
		if (!::IsTextureValid(static_cast<TextureBlock*>(result.data)->texture))
		{
			delete static_cast<TextureBlock*>(result.data);
			result.data = nullptr;
		}
		return result;
	}

	SharedShader ResourceCreator::CreateShader(::Shader shader) noexcept
	{
		SharedShader result;
		if (!::IsShaderValid(shader)) return result;
		result.data = new(std::nothrow) ShaderBlock(1, shader);
		if (!result.data) return result;
		if (!::IsShaderValid(static_cast<ShaderBlock*>(result.data)->shader))
		{
			delete static_cast<ShaderBlock*>(result.data);
			result.data = nullptr;
		}
		return result;
	}

	SharedImage ResourceCreator::CreateImage(::Image img) noexcept
	{
		SharedImage result;
		if (!::IsImageValid(img)) return result;
		result.data = new(std::nothrow) ImageBlock(1, img);
		if (!result.data) return result;
		if (!::IsImageValid(static_cast<ImageBlock*>(result.data)->image))
		{
			delete static_cast<ImageBlock*>(result.data);
			result.data = nullptr;
		}
		return result;
	}
}