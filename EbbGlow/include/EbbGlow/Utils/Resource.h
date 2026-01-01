#pragma once
#include <EbbGlow/Utils/Types.h>

namespace ebbglow::resource
{
	class SharedImage
	{
	private:
		void* image;
		size_t* ref;

		friend class ResourceCreator;

	public:
		SharedImage() noexcept;
		SharedImage(const char* imagePath) noexcept;
		SharedImage(const SharedImage& other);
		SharedImage(SharedImage&& other) noexcept;
		~SharedImage();

		SharedImage& operator=(const SharedImage& other);
		SharedImage& operator=(SharedImage&& other) noexcept;

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		int width() const noexcept;
		int height() const noexcept;

		void* get() noexcept { return image; }
		void* get() const noexcept { return image; }
	};

	class SharedTexture
	{
	private:
		void* texture;
		size_t* ref;

		friend class ResourceCreator;

	public:
		SharedTexture() noexcept;
		SharedTexture(const char* texturePath) noexcept;
		SharedTexture(const SharedTexture& other);
		SharedTexture(SharedTexture&& other) noexcept;
		~SharedTexture();

		SharedTexture& operator=(const SharedTexture& other);
		SharedTexture& operator=(SharedTexture&& other) noexcept;

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		int width() const noexcept;
		int height() const noexcept;

		void* get() noexcept { return texture; }
		void* get() const noexcept { return texture; }
	};

	using SharedTexture2D = SharedTexture;

	enum class FontType : uint8_t
	{
		Default,
		Sdf
	};

	class SharedFont
	{
	private:
		void* font;
		size_t* ref;

		friend class ResourceCreator;

	public:
		SharedFont() noexcept;
		SharedFont(const char* fontPath) noexcept;
		SharedFont(const SharedFont& other);
		SharedFont(SharedFont&& other) noexcept;
		~SharedFont();

		SharedFont& operator=(const SharedFont& other);
		SharedFont& operator=(SharedFont&& other) noexcept;

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		void* get() noexcept { return font; }
		void* get() const noexcept { return font; }
	};

	class SharedMusic
	{
	private:
		void* music;
		size_t* ref;

		friend class ResourceCreator;

	public:
		SharedMusic() noexcept;
		SharedMusic(const SharedMusic& other) noexcept;
		SharedMusic(SharedMusic&& other) noexcept;
		SharedMusic(const char* musicPath) noexcept;

		SharedMusic& operator=(const SharedMusic& other) noexcept;
		SharedMusic& operator=(SharedMusic&& other) noexcept;
		~SharedMusic();

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		void* get() noexcept { return music; }
		void* get() const noexcept { return music; }
	};

	class SharedShader
	{
	private:
		void* shader;
		size_t* ref;

		friend class ResourceCreator;

	public:
		SharedShader() noexcept;
		SharedShader(const SharedShader& other) noexcept;
		SharedShader(SharedShader&& other) noexcept;
		SharedShader(const char* shaderPath) noexcept;

		SharedShader& operator=(const SharedShader& other) noexcept;
		SharedShader& operator=(SharedShader&& other) noexcept;
		~SharedShader();

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		void* get() noexcept { return shader; }
		void* get() const noexcept { return shader; }
	};

	class SharedFile
	{
	private:
		unsigned char* fileData;
		size_t* ref;
		int dataSize;
		char* name;

		friend class ResourceCreator;

	public:
		class Iterator
		{
		private:
			size_t fileSize;
			int64_t offset;
			unsigned char* pointer;

		public:
			Iterator();
			Iterator(size_t size, unsigned char* data, int64_t offset = 0);
			Iterator(const SharedFile& file);
			Iterator(const SharedFile& file, int64_t offset);

			Iterator(const Iterator& other);
			Iterator(Iterator&& other) noexcept;

			Iterator& operator=(const Iterator& other) noexcept;
			Iterator& operator=(Iterator&& other) noexcept;

			Iterator& operator++() noexcept;
			Iterator operator++(int) noexcept;
			Iterator& operator--() noexcept;
			Iterator operator--(int) noexcept;

			unsigned char& operator*() noexcept;
			const unsigned char& operator*() const noexcept;
			unsigned char& operator[](int64_t _offset) noexcept;
			const unsigned char& operator[](int64_t _offset) const noexcept;

			bool eof() const noexcept;
			bool valid() const noexcept;
			void reset() noexcept;
			explicit operator bool() const noexcept;

			Iterator& operator+=(int64_t _offset) noexcept;
			Iterator& operator-=(int64_t _offset) noexcept;
			Iterator operator+(int64_t _offset) const noexcept;
			Iterator operator-(int64_t _offset) const noexcept;
			int64_t operator-(const Iterator& other) const noexcept;

			bool operator==(const Iterator& other) const noexcept;
			bool operator>(const Iterator& other) const noexcept;
			bool operator>=(const Iterator& other) const noexcept;
			bool operator<(const Iterator& other) const noexcept;
			bool operator<=(const Iterator& other) const noexcept;
			bool operator!=(const Iterator& other) const noexcept;

			size_t size() const noexcept;
			size_t remaining() const noexcept;
			int64_t position() const noexcept;
			unsigned char* get() const noexcept;
		};

		SharedFile() noexcept;
		SharedFile(const char* filePath);
		SharedFile(unsigned char* fileData, int dataSize, const char* name) noexcept;
		SharedFile(const SharedFile& other) noexcept;
		SharedFile(SharedFile&& other) noexcept;

		SharedFile& operator=(const SharedFile& other) noexcept;
		SharedFile& operator=(SharedFile&& other) noexcept;
		~SharedFile() noexcept;

		unsigned char* get() const noexcept { return fileData; }
		int size() const noexcept { return dataSize; }
		const char* fileName() const noexcept { return name; }
		operator bool() const noexcept { return ref; }
		bool valid() const noexcept { return ref; }

		Iterator begin();
		Iterator end();
	};

	class SharedRenderTexture
	{
	private:
		void* renderTexture;
		size_t* ref;

		friend class ResourceCreator;

	public:
		SharedRenderTexture() noexcept;
		SharedRenderTexture(const SharedRenderTexture& other) noexcept;
		SharedRenderTexture(SharedRenderTexture&& other) noexcept;
		SharedRenderTexture(int width, int height) noexcept;

		SharedRenderTexture& operator=(const SharedRenderTexture& other) noexcept;
		SharedRenderTexture& operator=(SharedRenderTexture&& other) noexcept;
		~SharedRenderTexture();

		operator bool() const noexcept { return ref; }
		bool valid() const noexcept { return ref; }

		int width() const noexcept;
		int height() const noexcept;

		void* get() noexcept { return renderTexture; }
		void* get() const noexcept { return renderTexture; }
	};

	typedef SharedRenderTexture SharedRenderTexture2D;

	class SharedSound
	{
	private:
		void* sound;
		size_t* ref;

		friend class ResourceCreator;

	public:
		SharedSound() noexcept;
		SharedSound(const SharedSound& other) noexcept;
		SharedSound(SharedSound&& other) noexcept;
		SharedSound(const char* soundPath) noexcept;

		SharedSound& operator=(const SharedSound& other) noexcept;
		SharedSound& operator=(SharedSound&& other) noexcept;
		~SharedSound();

		operator bool() const noexcept { return ref; }
		bool valid() const noexcept { return ref; }

		void* get() noexcept { return sound; }
		void* get() const noexcept { return sound; }
	};
}

namespace ebbglow
{
	namespace rsc = resource;
}