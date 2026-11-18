#pragma once

#include <EbbGlow/Resources/SharedFile.h>

namespace ebbglow::resource
{
	class SharedMusic
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedMusic() noexcept;
		SharedMusic(const SharedMusic& other) noexcept;
		SharedMusic(SharedMusic&& other) noexcept;
		SharedMusic(const std::filesystem::path& musicPath) noexcept;
		SharedMusic(const SharedFile& fileData) noexcept;

		SharedMusic& operator=(const SharedMusic& other) noexcept;
		SharedMusic& operator=(SharedMusic&& other) noexcept;
		~SharedMusic() noexcept;

		operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};
}