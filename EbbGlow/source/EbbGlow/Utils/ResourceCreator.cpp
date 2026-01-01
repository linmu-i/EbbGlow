#include "ResourceCreator.h"

namespace ebbglow::resource
{
	SharedFont ResourceCreator::CreateFont(::Font font) const noexcept
	{
		SharedFont result;
		if (font.texture.id == 0) return result;
		::Font * ptr = new(std::nothrow) ::Font(font);
		result.font = static_cast<void*>(ptr);
		result.ref = new(std::nothrow) size_t(1);
		return result;
	};

	SharedTexture ResourceCreator::CreateTexture(::Texture2D texture) const noexcept
	{
		SharedTexture result;
		if (texture.id == 0) return result;
		::Texture* ptr = new(std::nothrow) ::Texture(texture);
		result.texture = static_cast<void*>(ptr);
		result.ref = new(std::nothrow) size_t(1);
		return result;
	}

	SharedShader ResourceCreator::CreateShader(::Shader shader) const noexcept
	{
		SharedShader result;
		if (shader.id == 0) return result;
		::Shader* ptr = new(std::nothrow) ::Shader(shader);
		result.shader = static_cast<void*>(ptr);
		result.ref = new(std::nothrow) size_t(1);
		return result;
	}
}