#pragma once

#include <raylib.h>
#include <raymath.h>

namespace rlRAII
{
	class ImageRAII
	{
	private:
		Image image;
		size_t* ref;

	public:
		ImageRAII() noexcept : image({}), ref(nullptr) {}
		ImageRAII(const char* imagePath) noexcept
		{
			image = LoadImage(imagePath);
			if (image.data == nullptr)
			{
				ref = nullptr;
			}
			else
			{
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadImage(image);
					image = {};
				}
			}
		}
		ImageRAII(const Image img)
		{
			if (img.data == nullptr)
			{
				image = {};
				ref = nullptr;
			}
			else
			{
				image = img;
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadImage(image);
					image = {};
				}
			}
		}
		ImageRAII(const ImageRAII& other) : image(other.image), ref(other.ref)
		{
			if (ref)
			{
				++(*ref);
			}

		}
		ImageRAII(ImageRAII&& other) noexcept : image(other.image), ref(other.ref)
		{
			other.image = {};
			other.ref = nullptr;
		}
		~ImageRAII()
		{
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadImage(image);
					image = {};
					delete ref;
					ref = nullptr;
				}
			}
		}
		ImageRAII& operator=(const ImageRAII& other)
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadImage(image);
					delete ref;
				}
			}
			image = other.image;
			ref = other.ref;
			if (ref)
			{
				++(*ref);
			}
			return *this;
		}
		ImageRAII& operator=(ImageRAII&& other) noexcept
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadImage(image);
					delete ref;
				}
			}
			image = other.image;
			ref = other.ref;
			other.image = {};
			other.ref = nullptr;
			return *this;
		}
		Image& get() noexcept
		{
			return image;
		}
		const Image& get() const noexcept
		{
			return image;
		}
		operator bool() const noexcept
		{
			return ref != nullptr;
		}
		bool valid() const noexcept
		{
			return ref != nullptr;
		}
	};

	class TextureRAII
	{
	private:
		Texture2D texture;
		size_t* ref;

	public:
		TextureRAII() noexcept : texture({}), ref(nullptr) {}

		TextureRAII(const char* texturePath) noexcept
		{
			texture = LoadTexture(texturePath);
			if (texture.id == 0)
			{
				ref = nullptr;
			}
			else
			{
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadTexture(texture);
					texture = {};
				}
			}
		}

		TextureRAII(const Texture texture)
		{
			if (texture.id == 0)
			{
				this->texture = {};
				ref = nullptr;
			}
			else
			{
				this->texture = texture;
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadTexture(texture);
					this->texture = {};
				}
			}
		}

		TextureRAII(const TextureRAII& other) : texture(other.texture), ref(other.ref)
		{
			if (ref)
			{
				++(*ref);
			}
		}

		TextureRAII(TextureRAII&& other) noexcept
			: texture(other.texture), ref(other.ref)
		{
			other.texture = {};
			other.ref = nullptr;
		}

		~TextureRAII()
		{
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadTexture(texture);
					texture = {};
					delete ref;
					ref = nullptr;
				}
			}
		}

		TextureRAII& operator=(const TextureRAII& other)
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadTexture(texture);
					delete ref;
				}
			}
			texture = other.texture;
			ref = other.ref;
			if (ref)
			{
				++(*ref);
			}
			return *this;
		}

		TextureRAII& operator=(TextureRAII&& other) noexcept
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadTexture(texture);
					delete ref;
				}
			}
			texture = other.texture;
			ref = other.ref;
			other.texture = {};
			other.ref = nullptr;
			return *this;
		}

		Texture2D& get() noexcept
		{
			return texture;
		}

		const Texture2D& get() const noexcept
		{
			return texture;
		}

		operator bool() const noexcept
		{
			return ref != nullptr;
		}

		bool valid() const noexcept
		{
			return ref != nullptr;
		}
	};

	using Texture2DRAII = TextureRAII;

	class FontRAII
	{
	private:
		Font font;
		size_t* ref;

	public:
		FontRAII() noexcept : font({}), ref(nullptr) {}

		FontRAII(const char* fontPath) noexcept
		{
			if (fontPath == nullptr)
			{
				font = {};
				ref = nullptr;
				return;
			}
			font = LoadFont(fontPath);
			if (font.texture.id == 0) // 检查字体是否加载成功
			{
				ref = nullptr;
			}
			else
			{
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadFont(font);
					font = {};
				}
			}
		}

		FontRAII(const Font font)
		{
			if (font.texture.id == 0)
			{
				this->font = {};
				ref = nullptr;
			}
			else
			{
				this->font = font;
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadFont(font);
					this->font = {};
				}
			}
		}

		FontRAII(const FontRAII& other) : font(other.font), ref(other.ref)
		{
			if (ref)
			{
				++(*ref);
			}
		}

		FontRAII(FontRAII&& other) noexcept
			: font(other.font), ref(other.ref)
		{
			other.font = {};
			other.ref = nullptr;
		}

		~FontRAII()
		{
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadFont(font);
					font = {};
					delete ref;
					ref = nullptr;
				}
			}
		}

		FontRAII& operator=(const FontRAII& other)
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadFont(font);
					delete ref;
				}
			}
			font = other.font;
			ref = other.ref;
			if (ref)
			{
				++(*ref);
			}
			return *this;
		}

		FontRAII& operator=(FontRAII&& other) noexcept
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					UnloadFont(font);
					delete ref;
				}
			}
			font = other.font;
			ref = other.ref;
			other.font = {};
			other.ref = nullptr;
			return *this;
		}

		Font& get() noexcept
		{
			return font;
		}

		const Font& get() const noexcept
		{
			return font;
		}

		operator bool() const noexcept
		{
			return ref != nullptr;
		}

		bool valid() const noexcept
		{
			return ref != nullptr;
		}
	};



	class MusicRAII
	{
	private:
		Music music;
		size_t* ref;
		
	public:
		MusicRAII() noexcept : music({}), ref(nullptr) {}

		MusicRAII(const MusicRAII& other) noexcept
		{
			if (other.ref != nullptr)
			{
				music = other.music;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				music = {};
				ref = nullptr;
			}
		}

		MusicRAII(MusicRAII&& other) noexcept : music(other.music), ref(other.ref)
		{
			other.music = {};
			other.ref = nullptr;
		}

		MusicRAII(const char* musicPath) noexcept
		{
			music = LoadMusicStream(musicPath);
			if (IsMusicValid(music))
			{
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadMusicStream(music);
					music = {};
				}
			}
			else
			{
				ref = nullptr;
			}
		}

		MusicRAII(const Music otherMusic) noexcept
		{
			if (IsMusicValid(otherMusic))
			{
				music = otherMusic;
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadMusicStream(music);
					music = {};
				}
			}
			else
			{
				music = {};
				ref = nullptr;
			}
		}

		MusicRAII& operator=(const MusicRAII& other) noexcept
		{
			if (other.ref == ref)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadMusicStream(music);
				}
			}
			if (other.ref != nullptr)
			{
				music = other.music;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				music = {};
				ref = nullptr;
			}
		}

		MusicRAII& operator=(MusicRAII&& other) noexcept
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadMusicStream(music);
				}
			}
			music = other.music;
			ref = other.ref;
			other.music = {};
			other.ref = nullptr;
		}

		~MusicRAII()
		{
			if (ref != nullptr)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadMusicStream(music);
				}
				ref = nullptr;
				music = {};
			}
		}

		Music& get() noexcept
		{
			return music;
		}

		operator bool() const noexcept
		{
			return ref != nullptr;
		}

		bool valid() const noexcept
		{
			return ref != nullptr;
		}
	};



	class ShaderRAII
	{
	private:
		Shader shader;
		size_t* ref;

	public:
		ShaderRAII() noexcept : shader({}), ref(nullptr) {}

		ShaderRAII(const ShaderRAII& other) noexcept
		{
			if (other.ref != nullptr)
			{
				shader = other.shader;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				shader = {};
				ref = nullptr;
			}
		}

		ShaderRAII(ShaderRAII&& other) noexcept : shader(other.shader), ref(other.ref)
		{
			other.shader = {};
			other.ref = nullptr;
		}

		ShaderRAII(const char* shaderPath) noexcept
		{
			shader = LoadShader(0,shaderPath);
			if (IsShaderValid(shader))
			{
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadShader(shader);
					shader = {};
				}
			}
			else
			{
				ref = nullptr;
			}
		}

		ShaderRAII(const Shader otherShader) noexcept
		{
			if (IsShaderValid(otherShader))
			{
				shader = otherShader;
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadShader(shader);
					shader = {};
				}
			}
			else
			{
				shader = {};
				ref = nullptr;
			}
		}

		ShaderRAII& operator=(const ShaderRAII& other) noexcept
		{
			if (other.ref == ref)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadShader(shader);
				}
			}
			if (other.ref != nullptr)
			{
				shader = other.shader;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				shader = {};
				ref = nullptr;
			}
		}

		ShaderRAII& operator=(ShaderRAII&& other) noexcept
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadShader(shader);
				}
			}
			shader = other.shader;
			ref = other.ref;
			other.shader = {};
			other.ref = nullptr;
			return *this;
		}

		~ShaderRAII()
		{
			if (ref != nullptr)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadShader(shader);
				}
				ref = nullptr;
				shader = {};
			}
		}

		Shader& get() noexcept
		{
			return shader;
		}

		operator bool() const noexcept
		{
			return ref != nullptr;
		}

		bool valid() const noexcept
		{
			return ref != nullptr;
		}
	};

	class FileRAII
	{
	private:
		unsigned char* fileData;
		size_t* ref;
		int dataSize;
		char* name;

	public:
		class Iterator
		{
		private:
			size_t fileSize;
			int64_t offset;
			unsigned char* pointer;

		public:
			Iterator() : fileSize(0), offset(0), pointer(nullptr) {}
			Iterator(FileRAII file) : fileSize(file.dataSize), pointer(file.fileData), offset(0) {}
			Iterator(FileRAII file, int64_t offset) : fileSize(file.dataSize), pointer(file.fileData + offset), offset(offset) {}

			Iterator(const Iterator& other) : fileSize(other.fileSize), offset(other.offset), pointer(other.pointer) {}
			Iterator(Iterator&& other) : fileSize(other.fileSize), offset(other.offset), pointer(other.pointer)
			{
				other.fileSize = 0;
				other.offset = 0;
				other.pointer = nullptr;
			}

			Iterator& operator=(const Iterator& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}
				fileSize = other.fileSize;
				offset = other.offset;
				pointer = other.pointer;
				return *this;
			}

			Iterator& operator=(Iterator&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}
				fileSize = other.fileSize;
				offset = other.offset;
				pointer = other.pointer;
				other.fileSize = 0;
				other.offset = 0;
				other.pointer = nullptr;
				return *this;
			}

			Iterator& operator++() noexcept
			{
				++pointer;
				++offset;
				return *this;
			}

			Iterator operator++(int) noexcept
			{
				Iterator tmp = *this;
				++pointer;
				++offset;
				return tmp;
			}

			Iterator& operator--() noexcept
			{
				--pointer;
				--offset;
				return *this;
			}

			Iterator operator--(int) noexcept
			{
				Iterator tmp = *this;
				--pointer;
				--offset;
				return tmp;
			}

			unsigned char& operator*() noexcept
			{
				return *pointer;
			}

			const unsigned char& operator*() const noexcept
			{
				return *pointer;
			}

			unsigned char& operator[](int64_t _offset) noexcept
			{
				return pointer[_offset];
			}

			const unsigned char& operator[](int64_t _offset) const noexcept
			{
				return pointer[_offset];
			}

			unsigned char& at(int64_t _offset)
			{
				if (offset + _offset < 0)
				{
					throw std::out_of_range("Before File.");
				}
				if (offset + _offset >= fileSize)
				{
					throw std::out_of_range("After File.");
				}
				return pointer[_offset];
			}

			const unsigned char& at(int64_t _offset) const
			{
				if (offset + _offset < 0)
				{
					throw std::out_of_range("Before File.");
				}
				if (offset + _offset >= fileSize)
				{
					throw std::out_of_range("After File.");
				}
				return pointer[_offset];
			}

			bool eof() const noexcept
			{
				if (offset >= fileSize || offset < 0)
				{
					return true;
				}
				return false;
			}

			bool valid() const noexcept
			{
				return pointer;
			}

			void reset() noexcept
			{
				if (pointer)
				{
					pointer -= offset;
					offset = 0;
				}
			}

			explicit operator bool() const noexcept
			{
				return !eof();
			}

			Iterator& operator+=(int64_t _offset) noexcept
			{
				offset += _offset;
				pointer += _offset;
				return *this;
			}

			Iterator& operator-=(int64_t _offset) noexcept
			{
				offset -= _offset;
				pointer -= _offset;
				return *this;
			}

			Iterator operator+(int64_t _offset) const noexcept
			{
				Iterator result = *this;
				result.offset += _offset;
				result.pointer += _offset;
				return result;
			}

			Iterator operator-(int64_t _offset) const noexcept
			{
				Iterator result = *this;
				result.offset -= _offset;
				result.pointer -= _offset;
				return result;
			}

			int64_t operator-(const Iterator& other) const noexcept
			{
				return offset - other.offset;
			}

			bool operator==(const Iterator& other) const noexcept
			{
				return pointer == other.pointer;
			}

			bool operator>(const Iterator& other) const noexcept
			{
				return pointer > other.pointer;
			}

			bool operator>=(const Iterator& other) const noexcept
			{
				return pointer >= other.pointer;
			}

			bool operator<(const Iterator& other) const noexcept
			{
				return pointer < other.pointer;
			}

			bool operator<=(const Iterator& other) const noexcept
			{
				return pointer <= other.pointer;
			}

			bool operator!=(const Iterator& other) const noexcept
			{
				return pointer != other.pointer;
			}

			size_t size() const noexcept
			{
				return fileSize;
			}

			size_t remaining() const noexcept
			{
				return fileSize - offset - 1;
			}

			int64_t position() const noexcept
			{
				return offset;
			}

			unsigned char* get() const noexcept
			{
				return pointer;
			}
		};

		FileRAII() noexcept : ref(nullptr), fileData(nullptr), dataSize(0), name(nullptr) {}
		FileRAII(const char* filePath) : ref(new(std::nothrow) size_t(1))
		{
			name = new(std::nothrow) char[strlen(filePath) + 1];
			strcpy(name, filePath);
			fileData = LoadFileData(filePath, &dataSize);
		}
		FileRAII(unsigned char* fileData, int dataSize, const char* name) noexcept : ref(new(std::nothrow) size_t(1)), fileData(fileData), dataSize(dataSize)
		{
			this->name = new(std::nothrow) char[strlen(name) + 1];
			strcpy(this->name, name);
		}

		FileRAII(const FileRAII& other) noexcept : fileData(other.fileData), ref(other.ref), dataSize(other.dataSize), name(other.name)
		{
			if (ref)
			{
				++(*ref);
			}
		}
		FileRAII(FileRAII&& other) noexcept : fileData(other.fileData), ref(other.ref), dataSize(other.dataSize), name(other.name)
		{
			other.fileData = nullptr;
			other.ref = nullptr;
			other.name = nullptr;
			other.dataSize = 0;
		}

		FileRAII& operator=(const FileRAII& other) noexcept
		{
			if (ref == other.ref)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					delete[] name;
					UnloadFileData(fileData);
				}
			}
			ref = other.ref;
			fileData = other.fileData;
			dataSize = other.dataSize;
			name = other.name;
			if (ref)
			{
				++(*ref);
			}
			return *this;
		}
		FileRAII& operator=(FileRAII&& other) noexcept
		{
			if (ref == other.ref)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					delete[] name;
					UnloadFileData(fileData);
				}
			}
			ref = other.ref;
			fileData = other.fileData;
			dataSize = other.dataSize;
			name = other.name;
			other.fileData = nullptr;
			other.ref = nullptr;
			other.dataSize = 0;
			other.name = nullptr;
			return *this;
		}

		~FileRAII() noexcept
		{
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					delete[] name;
					UnloadFileData(fileData);
				}
				ref = nullptr;
				dataSize = 0;
				name = nullptr;
			}
		}

		unsigned char* get() const noexcept
		{
			return fileData;
		}

		int size() const noexcept 
		{
			return dataSize;
		}

		const char* fileName() const noexcept
		{
			return name;
		}

		operator bool() const noexcept
		{
			return ref;
		}

		bool valid() const noexcept
		{
			return ref;
		}

		Iterator begin()
		{
			return Iterator(*this);
		}
		
		Iterator end()
		{
			return Iterator(*this, dataSize);
		}
	};

	class RenderTextureRAII
	{
	private:
		RenderTexture renderTexture;
		size_t* ref;

	public:
		RenderTextureRAII() noexcept : renderTexture({}), ref(nullptr) {}

		RenderTextureRAII(const RenderTextureRAII& other) noexcept
		{
			if (other.ref)
			{
				renderTexture = other.renderTexture;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				renderTexture = {};
				ref = nullptr;
			}
		}

		RenderTextureRAII(RenderTextureRAII&& other) noexcept : renderTexture(other.renderTexture), ref(other.ref)
		{
			other.renderTexture = {};
			other.ref = nullptr;
		}

		RenderTextureRAII(int x, int y) noexcept
		{
			renderTexture = LoadRenderTexture(x, y);
			if (IsRenderTextureValid(renderTexture))
			{
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadRenderTexture(renderTexture);
					renderTexture = {};
				}
			}
			else
			{
				ref = nullptr;
			}
		}

		RenderTextureRAII(const RenderTexture otherRenderTexture) noexcept
		{
			if (IsRenderTextureValid(otherRenderTexture))
			{
				renderTexture = otherRenderTexture;
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadRenderTexture(renderTexture);
					renderTexture = {};
				}
			}
			else
			{
				renderTexture = {};
				ref = nullptr;
			}
		}

		RenderTextureRAII& operator=(const RenderTextureRAII& other) noexcept
		{
			if (other.ref == ref)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadRenderTexture(renderTexture);
				}
			}
			if (other.ref != nullptr)
			{
				renderTexture = other.renderTexture;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				renderTexture = {};
				ref = nullptr;
			}
		}

		RenderTextureRAII& operator=(RenderTextureRAII&& other) noexcept
		{
			if (&other == this)
			{
				return *this;
			}
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadRenderTexture(renderTexture);
				}
			}
			renderTexture = other.renderTexture;
			ref = other.ref;
			other.renderTexture = {};
			other.ref = nullptr;
			return *this;
		}

		~RenderTextureRAII()
		{
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadRenderTexture(renderTexture);
				}
				ref = nullptr;
				renderTexture = {};
			}
		}

		RenderTexture& get() noexcept
		{
			return renderTexture;
		}

		operator bool() const noexcept
		{
			return ref;
		}

		bool valid() const noexcept
		{
			return ref;
		}
	};

	typedef RenderTextureRAII RenderTexture2DRAII;
}