#pragma once

#include <EbbGlow/Resources/SharedImage.h>

namespace ebbglow::resource
{
	class SharedTexture
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedTexture() noexcept;
		SharedTexture(const std::filesystem::path& path) noexcept;
		SharedTexture(const SharedImage& img) noexcept;
		SharedTexture(const SharedTexture& other) noexcept;
		SharedTexture(SharedTexture&& other) noexcept;
		~SharedTexture() noexcept;

		SharedTexture& operator=(const SharedTexture& other) noexcept;
		SharedTexture& operator=(SharedTexture&& other) noexcept;

		explicit operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		int width() const noexcept;
		int height() const noexcept;
		Vec2 size() const noexcept;

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};

	using SharedTexture2D = SharedTexture;
}