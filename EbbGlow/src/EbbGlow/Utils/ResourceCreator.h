#include <raylib.h>
#include <EbbGlow/Utils/Resource.h>

namespace ebbglow::resource
{
	class ResourceCreator
	{
	public:
		static SharedFont CreateFont(::Font font) noexcept;
		static SharedTexture CreateTexture(::Texture2D texture) noexcept;
		static SharedShader CreateShader(::Shader shader) noexcept;
		static SharedImage CreateImage(::Image img) noexcept;
	};
}