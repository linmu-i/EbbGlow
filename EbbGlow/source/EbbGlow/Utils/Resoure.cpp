#include <EbbGlow/Utils/Resource.h>
#include <cstring>
#include <new>
#include <raylib.h>

namespace ebbglow::resource
{
	// SharedImage 实现
	SharedImage::SharedImage() noexcept : image(nullptr), ref(nullptr) {}

	SharedImage::SharedImage(const char* imagePath) noexcept
	{
		image = new(std::nothrow) ::Image(LoadImage(imagePath));
		if (image == nullptr)
		{
			ref = nullptr;
			return;
		}
		if (((::Image*)image)->data == nullptr)
		{
			delete (::Image*)image;
			ref = nullptr;
			return;
		}
		ref = new(std::nothrow) size_t(1);
		if (ref == nullptr)
		{
			UnloadImage(*((::Image*)image));
			delete (::Image*)image;
			image = nullptr;
		}
	}

	SharedImage::SharedImage(const SharedImage& other) : image(other.image), ref(other.ref)
	{
		if (ref)
		{
			++(*ref);
		}
	}

	SharedImage::SharedImage(SharedImage&& other) noexcept : image(other.image), ref(other.ref)
	{
		other.image = nullptr;
		other.ref = nullptr;
	}

	SharedImage::~SharedImage()
	{
		if (ref)
		{
			--(*ref);
			if (*ref == 0)
			{
				UnloadImage(*((::Image*)image));
				delete (::Image*)image;
				image = nullptr;
				delete ref;
				ref = nullptr;
			}
		}
	}

	SharedImage& SharedImage::operator=(const SharedImage& other)
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
				UnloadImage(*((::Image*)image));
				delete (::Image*)image;
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

	SharedImage& SharedImage::operator=(SharedImage&& other) noexcept
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
				UnloadImage(*((::Image*)image));
				delete (::Image*)image;
				delete ref;
			}
		}
		image = other.image;
		ref = other.ref;
		other.image = nullptr;
		other.ref = nullptr;
		return *this;
	}

	int SharedImage::width() const noexcept
	{
		return static_cast<Image*>(image)->width;
	}

	int SharedImage::height() const noexcept
	{
		return static_cast<Image*>(image)->height;
	}

	// SharedTexture 实现
	SharedTexture::SharedTexture() noexcept : texture(nullptr), ref(nullptr) {}

	SharedTexture::SharedTexture(const char* texturePath) noexcept
	{
		auto* loaded = new(std::nothrow) Texture2D(LoadTexture(texturePath));
		if (loaded == nullptr || loaded->id == 0)
		{
			if (loaded) delete loaded;
			texture = nullptr;
			ref = nullptr;
			return;
		}
		texture = loaded;
		ref = new(std::nothrow) size_t(1);
		if (ref == nullptr)
		{
			UnloadTexture(*static_cast<::Texture*>(texture));
			delete static_cast<::Texture*>(texture);
			texture = nullptr;
		}
	}

	SharedTexture::SharedTexture(const SharedTexture& other) : texture(other.texture), ref(other.ref)
	{
		if (ref)
		{
			++(*ref);
		}
	}

	SharedTexture::SharedTexture(SharedTexture&& other) noexcept
		: texture(other.texture), ref(other.ref)
	{
		other.texture = nullptr;
		other.ref = nullptr;
	}

	SharedTexture::~SharedTexture()
	{
		if (ref)
		{
			--(*ref);
			if (*ref == 0)
			{
				UnloadTexture(*static_cast<::Texture*>(texture));
				delete static_cast<::Texture*>(texture);
				texture = nullptr;
				delete ref;
				ref = nullptr;
			}
		}
	}

	SharedTexture& SharedTexture::operator=(const SharedTexture& other)
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
				UnloadTexture(*static_cast<::Texture*>(texture));
				delete static_cast<::Texture*>(texture);
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

	SharedTexture& SharedTexture::operator=(SharedTexture&& other) noexcept
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
				UnloadTexture(*static_cast<::Texture*>(texture));
				delete static_cast<::Texture*>(texture);
				delete ref;
			}
		}
		texture = other.texture;
		ref = other.ref;
		other.texture = nullptr;
		other.ref = nullptr;
		return *this;
	}

	int SharedTexture::width() const noexcept
	{
		if (texture == nullptr) return 0;
		return static_cast<Texture*>(texture)->width;
	}

	int SharedTexture::height() const noexcept
	{
		if (texture == nullptr) return 0;
		return static_cast<Texture*>(texture)->height;
	}

	// SharedFont 实现
	SharedFont::SharedFont() noexcept : font(nullptr), ref(nullptr) {}

	SharedFont::SharedFont(const char* fontPath) noexcept
	{
		if (fontPath == nullptr)
		{
			font = nullptr;
			ref = nullptr;
			return;
		}
		auto* loaded = new(std::nothrow) ::Font(LoadFont(fontPath));
		if (loaded == nullptr || loaded->texture.id == 0)
		{
			if (loaded) delete loaded;
			font = nullptr;
			ref = nullptr;
			return;
		}
		font = loaded;
		ref = new(std::nothrow) size_t(1);
		if (ref == nullptr)
		{
			UnloadFont(*static_cast<::Font*>(font));
			delete static_cast<::Font*>(font);
			font = nullptr;
		}
	}

	SharedFont::SharedFont(const SharedFont& other) : font(other.font), ref(other.ref)
	{
		if (ref)
		{
			++(*ref);
		}
	}

	SharedFont::SharedFont(SharedFont&& other) noexcept
		: font(other.font), ref(other.ref)
	{
		other.font = nullptr;
		other.ref = nullptr;
	}

	SharedFont::~SharedFont()
	{
		if (ref)
		{
			--(*ref);
			if (*ref == 0)
			{
				UnloadFont(*static_cast<::Font*>(font));
				delete static_cast<::Font*>(font);
				font = nullptr;
				delete ref;
				ref = nullptr;
			}
		}
	}

	SharedFont& SharedFont::operator=(const SharedFont& other)
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
				UnloadFont(*static_cast<::Font*>(font));
				delete static_cast<::Font*>(font);
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

	SharedFont& SharedFont::operator=(SharedFont&& other) noexcept
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
				UnloadFont(*static_cast<::Font*>(font));
				delete static_cast<::Font*>(font);
				delete ref;
			}
		}
		font = other.font;
		ref = other.ref;
		other.font = nullptr;
		other.ref = nullptr;
		return *this;
	}

	// SharedMusic 实现
	SharedMusic::SharedMusic() noexcept : music(nullptr), ref(nullptr) {}

	SharedMusic::SharedMusic(const SharedMusic& other) noexcept
	{
		if (other.ref != nullptr)
		{
			music = other.music;
			ref = other.ref;
			++(*ref);
		}
		else
		{
			music = nullptr;
			ref = nullptr;
		}
	}

	SharedMusic::SharedMusic(SharedMusic&& other) noexcept : music(other.music), ref(other.ref)
	{
		other.music = nullptr;
		other.ref = nullptr;
	}

	SharedMusic::SharedMusic(const char* musicPath) noexcept
	{
		auto* loaded = new(std::nothrow) ::Music(LoadMusicStream(musicPath));
		if (loaded == nullptr || !IsMusicValid(*loaded))
		{
			if (loaded) delete loaded;
			music = nullptr;
			ref = nullptr;
			return;
		}
		music = loaded;
		ref = new(std::nothrow) size_t(1);
		if (ref == nullptr)
		{
			UnloadMusicStream(*static_cast<::Music*>(music));
			delete static_cast<::Music*>(music);
			music = nullptr;
		}
	}

	SharedMusic& SharedMusic::operator=(const SharedMusic& other) noexcept
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
				UnloadMusicStream(*static_cast<::Music*>(music));
				delete static_cast<::Music*>(music);
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
			music = nullptr;
			ref = nullptr;
		}
		return *this;
	}
	
	SharedMusic& SharedMusic::operator=(SharedMusic&& other) noexcept
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
				UnloadMusicStream(*static_cast<::Music*>(music));
				delete static_cast<::Music*>(music);
			}
		}
		music = other.music;
		ref = other.ref;
		other.music = nullptr;
		other.ref = nullptr;
		return *this;
	}

	SharedMusic::~SharedMusic()
	{
		if (ref != nullptr)
		{
			--(*ref);
			if (*ref == 0)
			{
				delete ref;
				UnloadMusicStream(*static_cast<::Music*>(music));
				delete static_cast<::Music*>(music);
			}
			ref = nullptr;
			music = nullptr;
		}
	}

	// SharedShader 实现
	SharedShader::SharedShader() noexcept : shader(nullptr), ref(nullptr) {}

	SharedShader::SharedShader(const SharedShader& other) noexcept
	{
		if (other.ref != nullptr)
		{
			shader = other.shader;
			ref = other.ref;
			++(*ref);
		}
		else
		{
			shader = nullptr;
			ref = nullptr;
		}
	}

	SharedShader::SharedShader(SharedShader&& other) noexcept : shader(other.shader), ref(other.ref)
	{
		other.shader = nullptr;
		other.ref = nullptr;
	}

	SharedShader::SharedShader(const char* shaderPath) noexcept
	{
		auto* loaded = new(std::nothrow) ::Shader(LoadShader(0, shaderPath));
		if (loaded == nullptr || !IsShaderValid(*loaded))
		{
			if (loaded) delete loaded;
			shader = nullptr;
			ref = nullptr;
			return;
		}
		shader = loaded;
		ref = new(std::nothrow) size_t(1);
		if (ref == nullptr)
		{
			UnloadShader(*static_cast<::Shader*>(shader));
			delete static_cast<::Shader*>(shader);
			shader = nullptr;
		}
	}

	SharedShader& SharedShader::operator=(const SharedShader& other) noexcept
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
				UnloadShader(*static_cast<::Shader*>(shader));
				delete static_cast<::Shader*>(shader);
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
			shader = nullptr;
			ref = nullptr;
		}
		return *this;
	}

	SharedShader& SharedShader::operator=(SharedShader&& other) noexcept
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
				UnloadShader(*static_cast<::Shader*>(shader));
				delete static_cast<::Shader*>(shader);
			}
		}
		shader = other.shader;
		ref = other.ref;
		other.shader = nullptr;
		other.ref = nullptr;
		return *this;
	}

	SharedShader::~SharedShader()
	{
		if (ref != nullptr)
		{
			--(*ref);
			if (*ref == 0)
			{
				delete ref;
				UnloadShader(*static_cast<::Shader*>(shader));
				delete static_cast<::Shader*>(shader);
			}
			ref = nullptr;
			shader = nullptr;
		}
	}

	// SharedFile 实现
	SharedFile::SharedFile() noexcept : ref(nullptr), fileData(nullptr), dataSize(0), name(nullptr) {}

	SharedFile::SharedFile(const char* filePath) : ref(new(std::nothrow) size_t(1))
	{
		size_t len = strlen(filePath);
		name = new(std::nothrow) char[len + 1];
		memcpy(name, filePath, len + 1);
		fileData = LoadFileData(filePath, &dataSize);
	}

	SharedFile::SharedFile(unsigned char* fileData, int dataSize, const char* name) noexcept
		: ref(new(std::nothrow) size_t(1)), fileData(fileData), dataSize(dataSize)
	{
		size_t len = strlen(name);
		this->name = new(std::nothrow) char[len + 1];
		memcpy(this->name, name, len + 1);
	}

	SharedFile::SharedFile(const SharedFile& other) noexcept
		: fileData(other.fileData), ref(other.ref), dataSize(other.dataSize), name(other.name)
	{
		if (ref)
		{
			++(*ref);
		}
	}

	SharedFile::SharedFile(SharedFile&& other) noexcept
		: fileData(other.fileData), ref(other.ref), dataSize(other.dataSize), name(other.name)
	{
		other.fileData = nullptr;
		other.ref = nullptr;
		other.name = nullptr;
		other.dataSize = 0;
	}

	SharedFile& SharedFile::operator=(const SharedFile& other) noexcept
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

	SharedFile& SharedFile::operator=(SharedFile&& other) noexcept
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

	SharedFile::~SharedFile() noexcept
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

	SharedFile::Iterator SharedFile::begin()
	{
		return Iterator(*this);
	}

	SharedFile::Iterator SharedFile::end()
	{
		return Iterator(*this, dataSize);
	}

	// SharedFile::Iterator 实现
	SharedFile::Iterator::Iterator() : fileSize(0), offset(0), pointer(nullptr) {}

	SharedFile::Iterator::Iterator(size_t size, unsigned char* data, int64_t offset)
		: fileSize(size), pointer(data + offset), offset(offset) {
	}

	SharedFile::Iterator::Iterator(const SharedFile& file)
		: fileSize(file.dataSize), pointer(file.fileData), offset(0) {
	}

	SharedFile::Iterator::Iterator(const SharedFile& file, int64_t offset)
		: fileSize(file.dataSize), pointer(file.fileData + offset), offset(offset) {
	}

	SharedFile::Iterator::Iterator(const Iterator& other)
		: fileSize(other.fileSize), offset(other.offset), pointer(other.pointer) {
	}

	SharedFile::Iterator::Iterator(Iterator&& other) noexcept
		: fileSize(other.fileSize), offset(other.offset), pointer(other.pointer)
	{
		other.fileSize = 0;
		other.offset = 0;
		other.pointer = nullptr;
	}

	SharedFile::Iterator& SharedFile::Iterator::operator=(const Iterator& other) noexcept
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

	SharedFile::Iterator& SharedFile::Iterator::operator=(Iterator&& other) noexcept
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

	SharedFile::Iterator& SharedFile::Iterator::operator++() noexcept
	{
		++pointer;
		++offset;
		return *this;
	}

	SharedFile::Iterator SharedFile::Iterator::operator++(int) noexcept
	{
		Iterator tmp = *this;
		++pointer;
		++offset;
		return tmp;
	}

	SharedFile::Iterator& SharedFile::Iterator::operator--() noexcept
	{
		--pointer;
		--offset;
		return *this;
	}

	SharedFile::Iterator SharedFile::Iterator::operator--(int) noexcept
	{
		Iterator tmp = *this;
		--pointer;
		--offset;
		return tmp;
	}

	unsigned char& SharedFile::Iterator::operator*() noexcept
	{
		return *pointer;
	}

	const unsigned char& SharedFile::Iterator::operator*() const noexcept
	{
		return *pointer;
	}

	unsigned char& SharedFile::Iterator::operator[](int64_t _offset) noexcept
	{
		return pointer[_offset];
	}

	const unsigned char& SharedFile::Iterator::operator[](int64_t _offset) const noexcept
	{
		return pointer[_offset];
	}

	bool SharedFile::Iterator::eof() const noexcept
	{
		if (offset >= fileSize || offset < 0)
		{
			return true;
		}
		return false;
	}

	bool SharedFile::Iterator::valid() const noexcept
	{
		return pointer;
	}

	void SharedFile::Iterator::reset() noexcept
	{
		if (pointer)
		{
			pointer -= offset;
			offset = 0;
		}
	}

	SharedFile::Iterator::operator bool() const noexcept
	{
		return !eof();
	}

	SharedFile::Iterator& SharedFile::Iterator::operator+=(int64_t _offset) noexcept
	{
		offset += _offset;
		pointer += _offset;
		return *this;
	}

	SharedFile::Iterator& SharedFile::Iterator::operator-=(int64_t _offset) noexcept
	{
		offset -= _offset;
		pointer -= _offset;
		return *this;
	}

	SharedFile::Iterator SharedFile::Iterator::operator+(int64_t _offset) const noexcept
	{
		Iterator result = *this;
		result.offset += _offset;
		result.pointer += _offset;
		return result;
	}

	SharedFile::Iterator SharedFile::Iterator::operator-(int64_t _offset) const noexcept
	{
		Iterator result = *this;
		result.offset -= _offset;
		result.pointer -= _offset;
		return result;
	}

	int64_t SharedFile::Iterator::operator-(const Iterator& other) const noexcept
	{
		return offset - other.offset;
	}

	bool SharedFile::Iterator::operator==(const Iterator& other) const noexcept
	{
		return pointer == other.pointer;
	}

	bool SharedFile::Iterator::operator>(const Iterator& other) const noexcept
	{
		return pointer > other.pointer;
	}

	bool SharedFile::Iterator::operator>=(const Iterator& other) const noexcept
	{
		return pointer >= other.pointer;
	}

	bool SharedFile::Iterator::operator<(const Iterator& other) const noexcept
	{
		return pointer < other.pointer;
	}

	bool SharedFile::Iterator::operator<=(const Iterator& other) const noexcept
	{
		return pointer <= other.pointer;
	}

	bool SharedFile::Iterator::operator!=(const Iterator& other) const noexcept
	{
		return pointer != other.pointer;
	}

	size_t SharedFile::Iterator::size() const noexcept
	{
		return fileSize;
	}

	size_t SharedFile::Iterator::remaining() const noexcept
	{
		return fileSize - offset - 1;
	}

	int64_t SharedFile::Iterator::position() const noexcept
	{
		return offset;
	}

	unsigned char* SharedFile::Iterator::get() const noexcept
	{
		return pointer;
	}

	// SharedRenderTexture 实现
	SharedRenderTexture::SharedRenderTexture() noexcept : renderTexture(nullptr), ref(nullptr) {}

	SharedRenderTexture::SharedRenderTexture(const SharedRenderTexture& other) noexcept
	{
		if (other.ref)
		{
			renderTexture = other.renderTexture;
			ref = other.ref;
			++(*ref);
		}
		else
		{
			renderTexture = nullptr;
			ref = nullptr;
		}
	}

	SharedRenderTexture::SharedRenderTexture(SharedRenderTexture&& other) noexcept
		: renderTexture(other.renderTexture), ref(other.ref)
	{
		other.renderTexture = nullptr;
		other.ref = nullptr;
	}

	SharedRenderTexture::SharedRenderTexture(int x, int y) noexcept
	{
		auto* loaded = new(std::nothrow) ::RenderTexture(LoadRenderTexture(x, y));
		if (loaded == nullptr || !IsRenderTextureValid(*loaded))
		{
			if (loaded) delete loaded;
			renderTexture = nullptr;
			ref = nullptr;
			return;
		}
		renderTexture = loaded;
		ref = new(std::nothrow) size_t(1);
		if (ref == nullptr)
		{
			UnloadRenderTexture(*static_cast<::RenderTexture*>(renderTexture));
			delete static_cast<::RenderTexture*>(renderTexture);
			renderTexture = nullptr;
		}
	}

	SharedRenderTexture& SharedRenderTexture::operator=(const SharedRenderTexture& other) noexcept
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
				UnloadRenderTexture(*static_cast<::RenderTexture*>(renderTexture));
				delete static_cast<::RenderTexture*>(renderTexture);
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
			renderTexture = nullptr;
			ref = nullptr;
		}
		return *this;
	}

	SharedRenderTexture& SharedRenderTexture::operator=(SharedRenderTexture&& other) noexcept
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
				UnloadRenderTexture(*static_cast<::RenderTexture*>(renderTexture));
				delete static_cast<::RenderTexture*>(renderTexture);
			}
		}
		renderTexture = other.renderTexture;
		ref = other.ref;
		other.renderTexture = nullptr;
		other.ref = nullptr;
		return *this;
	}

	SharedRenderTexture::~SharedRenderTexture()
	{
		if (ref)
		{
			--(*ref);
			if (*ref == 0)
			{
				delete ref;
				UnloadRenderTexture(*static_cast<::RenderTexture*>(renderTexture));
				delete static_cast<::RenderTexture*>(renderTexture);
			}
			ref = nullptr;
			renderTexture = nullptr;
		}
	}

	int SharedRenderTexture::width() const noexcept
	{
		return static_cast<::RenderTexture*>(renderTexture)->texture.width;
	}

	int SharedRenderTexture::height() const noexcept
	{
		return static_cast<::RenderTexture*>(renderTexture)->texture.height;
	}

	// SharedSound 实现
	SharedSound::SharedSound() noexcept : sound(nullptr), ref(nullptr) {}

	SharedSound::SharedSound(const SharedSound& other) noexcept
	{
		if (other.ref)
		{
			sound = other.sound;
			ref = other.ref;
			++(*ref);
		}
		else
		{
			sound = nullptr;
			ref = nullptr;
		}
	}

	SharedSound::SharedSound(SharedSound&& other) noexcept : sound(other.sound), ref(other.ref)
	{
		other.sound = nullptr;
		other.ref = nullptr;
	}

	SharedSound::SharedSound(const char* soundPath) noexcept
	{
		auto* loaded = new(std::nothrow) ::Sound(LoadSound(soundPath));
		if (loaded == nullptr || !IsSoundValid(*loaded))
		{
			if (loaded) delete loaded;
			sound = nullptr;
			ref = nullptr;
			return;
		}
		sound = loaded;
		ref = new(std::nothrow) size_t(1);
		if (ref == nullptr)
		{
			UnloadSound(*static_cast<::Sound*>(sound));
			delete static_cast<::Sound*>(sound);
			sound = nullptr;
		}
	}

	SharedSound& SharedSound::operator=(const SharedSound& other) noexcept
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
				UnloadSound(*static_cast<::Sound*>(sound));
				delete static_cast<::Sound*>(sound);
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
			sound = nullptr;
			ref = nullptr;
		}
		return *this;
	}

	SharedSound& SharedSound::operator=(SharedSound&& other) noexcept
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
				UnloadSound(*static_cast<::Sound*>(sound));
				delete static_cast<::Sound*>(sound);
			}
		}
		sound = other.sound;
		ref = other.ref;
		other.sound = nullptr;
		other.ref = nullptr;
		return *this;
	}

	SharedSound::~SharedSound()
	{
		if (ref)
		{
			--(*ref);
			if (*ref == 0)
			{
				delete ref;
				UnloadSound(*static_cast<::Sound*>(sound));
				delete static_cast<::Sound*>(sound);
			}
			ref = nullptr;
			sound = nullptr;
		}
	}
}