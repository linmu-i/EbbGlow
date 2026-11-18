#pragma once

#include <EbbGlow/Resources/SharedFile.h>

namespace ebbglow::resource
{
	class SharedShader
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedShader() noexcept;
		SharedShader(const SharedShader& other) noexcept;
		SharedShader(SharedShader&& other) noexcept;
		SharedShader(const std::filesystem::path& vs, const std::filesystem::path& fs) noexcept;
		SharedShader(const SharedFile& vs, const SharedFile& fs) noexcept;

		SharedShader& operator=(const SharedShader& other) noexcept;
		SharedShader& operator=(SharedShader&& other) noexcept;
		~SharedShader() noexcept;

		operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};
}