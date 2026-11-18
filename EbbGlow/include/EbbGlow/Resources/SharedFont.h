#pragma once

#include <EbbGlow/Resources/SharedFile.h>

namespace ebbglow::resource
{
	enum class FontType : uint8_t
	{
		Default,
		Sdf
	};

	class SharedFont
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedFont() noexcept;
		SharedFont(const std::filesystem::path&, float fontSize, std::vector<int32_t> codepoints = {}) noexcept;//码点数组值传递，需要进行去重修正以及其他处理
		SharedFont(const SharedFile& fileData, float fontSize, std::vector<int32_t> codepoints = {}) noexcept;//码点数组值传递，需要进行去重修正以及其他处理
		SharedFont(const SharedFont& other) noexcept;
		SharedFont(SharedFont&& other) noexcept;
		~SharedFont() noexcept;

		SharedFont& operator=(const SharedFont& other) noexcept;
		SharedFont& operator=(SharedFont&& other) noexcept;

		operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};
}