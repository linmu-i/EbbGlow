#pragma once

#include <EbbGlow/Resources/SharedTexture.h>

namespace ebbglow::resource
{
	class SharedRenderTexture
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedRenderTexture() noexcept;
		SharedRenderTexture(const SharedRenderTexture& other) noexcept;
		SharedRenderTexture(SharedRenderTexture&& other) noexcept;
		SharedRenderTexture(int width, int height) noexcept;

		SharedRenderTexture& operator=(const SharedRenderTexture& other) noexcept;
		SharedRenderTexture& operator=(SharedRenderTexture&& other) noexcept;
		~SharedRenderTexture();

		operator bool() const noexcept { return data; }
		bool valid() const noexcept { return data; }

		int width() const noexcept;
		int height() const noexcept;
		Vec2 size() const noexcept;

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;

		SharedTexture& texture() noexcept;
		const SharedTexture& texture() const noexcept;
		SharedTexture& depth() noexcept;
		const SharedTexture& depth() const noexcept;
	};

	typedef SharedRenderTexture SharedRenderTexture2D;
}