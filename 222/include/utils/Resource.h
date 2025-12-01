#pragma once

#include <raylib.h>
#include <raymath.h>

namespace resource
{
	class SharedImage
	{
	private:
		Image image;
		size_t* ref;

	public:
		SharedImage() noexcept : image({}), ref(nullptr) {}
		SharedImage(const char* imagePath) noexcept
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
		SharedImage(const Image img)
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
		SharedImage(const SharedImage& other) : image(other.image), ref(other.ref)
		{
			if (ref)
			{
				++(*ref);
			}

		}
		SharedImage(SharedImage&& other) noexcept : image(other.image), ref(other.ref)
		{
			other.image = {};
			other.ref = nullptr;
		}
		~SharedImage()
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
		SharedImage& operator=(const SharedImage& other)
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
		SharedImage& operator=(SharedImage&& other) noexcept
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

	class SharedTexture
	{
	private:
		Texture2D texture;
		size_t* ref;

	public:
		SharedTexture() noexcept : texture({}), ref(nullptr) {}

		SharedTexture(const char* texturePath) noexcept
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

		SharedTexture(const Texture texture)
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

		SharedTexture(const SharedTexture& other) : texture(other.texture), ref(other.ref)
		{
			if (ref)
			{
				++(*ref);
			}
		}

		SharedTexture(SharedTexture&& other) noexcept
			: texture(other.texture), ref(other.ref)
		{
			other.texture = {};
			other.ref = nullptr;
		}

		~SharedTexture()
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

		SharedTexture& operator=(const SharedTexture& other)
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

		SharedTexture& operator=(SharedTexture&& other) noexcept
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

	using SharedTexture2D = SharedTexture;

	class SharedFont
	{
	private:
		Font font;
		size_t* ref;

	public:
		SharedFont() noexcept : font({}), ref(nullptr) {}

		SharedFont(const char* fontPath) noexcept
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

		SharedFont(const Font font)
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

		SharedFont(const SharedFont& other) : font(other.font), ref(other.ref)
		{
			if (ref)
			{
				++(*ref);
			}
		}

		SharedFont(SharedFont&& other) noexcept
			: font(other.font), ref(other.ref)
		{
			other.font = {};
			other.ref = nullptr;
		}

		~SharedFont()
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

		SharedFont& operator=(const SharedFont& other)
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

		SharedFont& operator=(SharedFont&& other) noexcept
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



	class SharedMusic
	{
	private:
		Music music;
		size_t* ref;
		
	public:
		SharedMusic() noexcept : music({}), ref(nullptr) {}

		SharedMusic(const SharedMusic& other) noexcept
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

		SharedMusic(SharedMusic&& other) noexcept : music(other.music), ref(other.ref)
		{
			other.music = {};
			other.ref = nullptr;
		}

		SharedMusic(const char* musicPath) noexcept
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

		SharedMusic(const Music otherMusic) noexcept
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

		SharedMusic& operator=(const SharedMusic& other) noexcept
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

		SharedMusic& operator=(SharedMusic&& other) noexcept
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

		~SharedMusic()
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



	class SharedShader
	{
	private:
		Shader shader;
		size_t* ref;

	public:
		SharedShader() noexcept : shader({}), ref(nullptr) {}

		SharedShader(const SharedShader& other) noexcept
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

		SharedShader(SharedShader&& other) noexcept : shader(other.shader), ref(other.ref)
		{
			other.shader = {};
			other.ref = nullptr;
		}

		SharedShader(const char* shaderPath) noexcept
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

		SharedShader(const Shader otherShader) noexcept
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

		SharedShader& operator=(const SharedShader& other) noexcept
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

		SharedShader& operator=(SharedShader&& other) noexcept
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

		~SharedShader()
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

	class SharedFile
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
			Iterator(size_t size, unsigned char* data, int64_t offset = 0) : fileSize(size), pointer(data), offset(offset) {}
			Iterator(const SharedFile& file) : fileSize(file.dataSize), pointer(file.fileData), offset(0) {}
			Iterator(const SharedFile& file, int64_t offset) : fileSize(file.dataSize), pointer(file.fileData + offset), offset(offset) {}

			Iterator(const Iterator& other) : fileSize(other.fileSize), offset(other.offset), pointer(other.pointer) {}
			Iterator(Iterator&& other) noexcept : fileSize(other.fileSize), offset(other.offset), pointer(other.pointer)
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

		SharedFile() noexcept : ref(nullptr), fileData(nullptr), dataSize(0), name(nullptr) {}
		SharedFile(const char* filePath) : ref(new(std::nothrow) size_t(1))
		{
			name = new(std::nothrow) char[strlen(filePath) + 1];
			strcpy(name, filePath);
			fileData = LoadFileData(filePath, &dataSize);
		}
		SharedFile(unsigned char* fileData, int dataSize, const char* name) noexcept : ref(new(std::nothrow) size_t(1)), fileData(fileData), dataSize(dataSize)
		{
			this->name = new(std::nothrow) char[strlen(name) + 1];
			strcpy(this->name, name);
		}
		
		SharedFile(const SharedFile& other) noexcept : fileData(other.fileData), ref(other.ref), dataSize(other.dataSize), name(other.name)
		{
			if (ref)
			{
				++(*ref);
			}
		}
		SharedFile(SharedFile&& other) noexcept : fileData(other.fileData), ref(other.ref), dataSize(other.dataSize), name(other.name)
		{
			other.fileData = nullptr;
			other.ref = nullptr;
			other.name = nullptr;
			other.dataSize = 0;
		}

		SharedFile& operator=(const SharedFile& other) noexcept
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
		SharedFile& operator=(SharedFile&& other) noexcept
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

		~SharedFile() noexcept
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

	class SharedRenderTexture
	{
	private:
		RenderTexture renderTexture;
		size_t* ref;

	public:
		SharedRenderTexture() noexcept : renderTexture({}), ref(nullptr) {}

		SharedRenderTexture(const SharedRenderTexture& other) noexcept
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

		SharedRenderTexture(SharedRenderTexture&& other) noexcept : renderTexture(other.renderTexture), ref(other.ref)
		{
			other.renderTexture = {};
			other.ref = nullptr;
		}

		SharedRenderTexture(int x, int y) noexcept
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

		SharedRenderTexture(const RenderTexture otherRenderTexture) noexcept
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

		SharedRenderTexture& operator=(const SharedRenderTexture& other) noexcept
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

		SharedRenderTexture& operator=(SharedRenderTexture&& other) noexcept
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

		~SharedRenderTexture()
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

	typedef SharedRenderTexture SharedRenderTexture2D;

	class SharedSound
	{
	private:
		Sound sound;
		size_t* ref;

	public:
		SharedSound() noexcept : sound({}), ref(nullptr) {}

		SharedSound(const SharedSound& other) noexcept
		{
			if (other.ref)
			{
				sound = other.sound;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				sound = {};
				ref = nullptr;
			}
		}

		SharedSound(SharedSound&& other) noexcept : sound(other.sound), ref(other.ref)
		{
			other.sound = {};
			other.ref = nullptr;
		}

		SharedSound(const char* soundPath) noexcept
		{
			sound = LoadSound(soundPath);
			if (IsSoundValid(sound))
			{
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadSound(sound);
					sound = {};
				}
			}
			else
			{
				ref = nullptr;
			}
		}

		SharedSound(const Sound otherSound) noexcept
		{
			if (IsSoundValid(otherSound))
			{
				sound = otherSound;
				ref = new(std::nothrow) size_t(1);
				if (ref == nullptr)
				{
					UnloadSound(sound);
					sound = {};
				}
			}
			else
			{
				sound = {};
				ref = nullptr;
			}
		}

		SharedSound& operator=(const SharedSound& other) noexcept
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
					UnloadSound(sound);
				}
			}
			if (other.ref != nullptr)
			{
				sound = other.sound;
				ref = other.ref;
				++(*ref);
			}
			else
			{
				sound = {};
				ref = nullptr;
			}
		}

		SharedSound& operator=(SharedSound&& other) noexcept
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
					UnloadSound(sound);
				}
			}
			sound = other.sound;
			ref = other.ref;
			other.sound = {};
			other.ref = nullptr;
			return *this;
		}

		~SharedSound()
		{
			if (ref)
			{
				--(*ref);
				if (*ref == 0)
				{
					delete ref;
					UnloadSound(sound);
				}
				ref = nullptr;
				sound = {};
			}
		}

		Sound& get() noexcept
		{
			return sound;
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
}

namespace rsc = resource;