#include <raylib.h>
#include <EbbGlow/Utils/Resource.h>

namespace ebbglow::resource
{
	class ResourceCreator
	{
	public:
		SharedFont CreateFont(::Font font) const noexcept;
		SharedTexture CreateTexture(::Texture2D texture) const noexcept;
		SharedShader CreateShader(::Shader shader) const noexcept;
	};
}