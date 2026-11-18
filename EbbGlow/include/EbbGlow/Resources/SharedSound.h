#pragma once

#include <EbbGlow/Resources/SharedFile.h>

namespace ebbglow::resource
{
	class SharedSound
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedSound() noexcept;
		SharedSound(const SharedSound& other) noexcept;
		SharedSound(SharedSound&& other) noexcept;
		SharedSound(const std::filesystem::path& soundPath) noexcept;

		SharedSound& operator=(const SharedSound& other) noexcept;
		SharedSound& operator=(SharedSound&& other) noexcept;
		~SharedSound() noexcept;

		operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};
}