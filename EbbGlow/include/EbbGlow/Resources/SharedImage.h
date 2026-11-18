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

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};
}