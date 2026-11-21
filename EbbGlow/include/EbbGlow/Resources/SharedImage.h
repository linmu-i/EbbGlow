#pragma once

#include <EbbGlow/Resources/SharedFile.h>
#include <EbbGlow/Utils/Types.h>

namespace ebbglow::resource
{
	class SharedImage
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedImage() noexcept;
		SharedImage(const std::filesystem::path& path) noexcept;
		SharedImage(const SharedFile& file) noexcept;
		SharedImage(const SharedImage& other) noexcept;
		SharedImage(SharedImage&& other) noexcept;
		~SharedImage() noexcept;

		SharedImage& operator=(const SharedImage& other) noexcept;
		SharedImage& operator=(SharedImage&& other) noexcept;

		explicit operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		int width() const noexcept;
		int height() const noexcept;
		Vec2 size() const noexcept;
		PixelFormat pixelFormat() const noexcept;
		uint8_t* pixelData() noexcept;
		const uint8_t* pixelData() const noexcept;

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};

	enum class PixelFormat : uint8_t
	{
		R8G8B8A8,
		GrayScale,
		Other
	};
}