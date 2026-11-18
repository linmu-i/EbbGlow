#include <cstring>
#include <new>
#include <filesystem>
#include <string>
#include <fstream>
#include <raylib.h>

#include <EbbGlow/Resources/Resources.h>
#include "ResourceCreator.h"
#include "ResourceBlock.h"

namespace ebbglow::resource
{
#pragma region SharedImage 实现

	SharedImage::SharedImage() noexcept : data(nullptr) {}

	SharedImage::SharedImage(const std::filesystem::path& path) noexcept : data(nullptr)
	{
		data = new(std::nothrow) ImageBlock(1, ::LoadImage(reinterpret_cast<const char*>(path.u8string().c_str())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsImageValid(((ImageBlock*)data)->image))
		{
			delete (ImageBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedImage::SharedImage(const SharedFile& file) noexcept : data(nullptr)
	{
		if (!file.valid())
		{
			return;
		}
		data = new(std::nothrow) ImageBlock(1, ::LoadImageFromMemory(reinterpret_cast<const char*>(file.fileExtension().u8string().c_str()), file.get(), file.size()));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsImageValid(((ImageBlock*)data)->image))
		{
			delete (ImageBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedImage::SharedImage(const SharedImage& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(((ImageBlock*)data)->refCnt);
		}
	}

	SharedImage::SharedImage(SharedImage&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedImage::~SharedImage() noexcept
	{
		release();
	}

	SharedImage& SharedImage::operator=(const SharedImage& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(((ImageBlock*)data)->refCnt);
		}
		return *this;
	}

	SharedImage& SharedImage::operator=(SharedImage&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	int SharedImage::width() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<ImageBlock*>(data)->image.width;
	}

	int SharedImage::height() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<ImageBlock*>(data)->image.height;
	}

	void* SharedImage::get() noexcept
	{
		if (data == nullptr) return nullptr;
		return &(static_cast<ImageBlock*>(data)->image);
	}

	const void* SharedImage::get() const noexcept
	{
		if (data == nullptr) return nullptr;
		return &(static_cast<ImageBlock*>(data)->image);
	}

	void SharedImage::release() noexcept
	{
		if (data)
		{
			--(((ImageBlock*)data)->refCnt);
			if (((ImageBlock*)data)->refCnt == 0)
			{
				UnloadImage(((ImageBlock*)data)->image);
				delete (ImageBlock*)data;
			}
			data = nullptr;
		}
	}

#pragma endregion

#pragma region SharedTexture 实现

	SharedTexture::SharedTexture() noexcept : data(nullptr) {}

	SharedTexture::SharedTexture(const std::filesystem::path& path) noexcept : data(nullptr)
	{
		data = new(std::nothrow) TextureBlock(1, ::LoadTexture(reinterpret_cast<const char*>(path.u8string().c_str())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsTextureValid(((TextureBlock*)data)->texture))
		{
			delete (TextureBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedTexture::SharedTexture(const SharedImage& image) noexcept : data(nullptr)
	{
		if (!image.valid())
		{
			return;
		}
		data = new(std::nothrow) TextureBlock(1, ::LoadTextureFromImage(*reinterpret_cast<const ::Image*>(image.get())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsTextureValid(((TextureBlock*)data)->texture))
		{
			delete (TextureBlock*)data;
			data = nullptr;
			return;
		}
	}

	SharedTexture::SharedTexture(const SharedTexture& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<TextureBlock*>(data)->refCnt);
		}
	}

	SharedTexture::SharedTexture(SharedTexture&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedTexture::~SharedTexture() noexcept
	{
		release();
	}

	SharedTexture& SharedTexture::operator=(const SharedTexture& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(static_cast<TextureBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedTexture& SharedTexture::operator=(SharedTexture&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	int SharedTexture::width() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<TextureBlock*>(data)->texture.width;
	}

	int SharedTexture::height() const noexcept
	{
		if (data == nullptr) return 0;
		return static_cast<TextureBlock*>(data)->texture.height;
	}

	Vec2 SharedTexture::size() const noexcept
	{
		if (data == nullptr) return Vec2{ 0.0f, 0.0f };
		return Vec2{ static_cast<float>(width()), static_cast<float>(height())};
	}

	void SharedTexture::release() noexcept
	{
		if (data)
		{
			--(((TextureBlock*)data)->refCnt);
			if (((TextureBlock*)data)->refCnt == 0)
			{
				UnloadTexture(((TextureBlock*)data)->texture);
				delete (TextureBlock*)data;
			}
			data = nullptr;
		}
	}

	void* SharedTexture::get() noexcept
	{
		if (data) return &(static_cast<TextureBlock*>(data)->texture);
		return nullptr;
	}

	const void* SharedTexture::get() const noexcept
	{
		if (data) return &(static_cast<TextureBlock*>(data)->texture);
		return nullptr;
	}

#pragma endregion

#pragma region SharedFont 实现

	SharedFont::SharedFont() noexcept : data(nullptr) {}

	SharedFont::SharedFont(const std::filesystem::path& path, float fontSize, std::vector<int32_t> codepoints) noexcept
	{
		if (codepoints.empty())
		{
			codepoints.reserve(98);
			for (int32_t i = 32; i < 127; ++i)
			{
				codepoints.push_back(i);
			}
		}
		codepoints.reserve(codepoints.size() + 3);
		codepoints.push_back('1');
		codepoints.push_back('1');
		codepoints.push_back('1');
		data = new(std::nothrow) FontBlock(1, LoadFontEx(reinterpret_cast<const char*>(path.u8string().c_str()), fontSize, codepoints.data(), codepoints.size()));
		if (!data) return;
		if (!::IsFontValid(static_cast<FontBlock*>(data)->font))
		{
			delete static_cast<FontBlock*>(data);
			data = nullptr;
		}
	}

	SharedFont::SharedFont(const SharedFile& fileData, float fontSize, std::vector<int32_t> codepoints) noexcept : data(nullptr)
	{
		if (!fileData.valid()) return;
		if (codepoints.empty())
		{
			codepoints.reserve(98);
			for (int32_t i = 32; i < 127; ++i)
			{
				codepoints.push_back(i);
			}
		}
		codepoints.reserve(codepoints.size() + 3);
		codepoints.push_back('1');
		codepoints.push_back('1');
		codepoints.push_back('1');
		data = new(std::nothrow) FontBlock(1, LoadFontFromMemory(fileData.fileExtension().string().c_str(), fileData.get(), fileData.size(), fontSize, codepoints.data(), codepoints.size()));
		if (!data) return;
		if (!::IsFontValid(static_cast<FontBlock*>(data)->font))
		{
			delete static_cast<FontBlock*>(data);
			data = nullptr;
		}
	}

	SharedFont::SharedFont(const SharedFont& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<FontBlock*>(data)->refCnt);
		}
	}

	SharedFont::SharedFont(SharedFont&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedFont::~SharedFont()
	{
		release();
	}

	SharedFont& SharedFont::operator=(const SharedFont& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(static_cast<FontBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedFont& SharedFont::operator=(SharedFont&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	void SharedFont::release() noexcept
	{
		if (data)
		{
			--(static_cast<FontBlock*>(data)->refCnt);
			if (static_cast<FontBlock*>(data)->refCnt == 0)
			{
				UnloadFont(static_cast<FontBlock*>(data)->font);
				delete static_cast<FontBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedFont::get() noexcept
	{
		if (data) return &(static_cast<FontBlock*>(data)->font);
		return nullptr;
	}

	const void* SharedFont::get() const noexcept
	{
		if (data) return &(static_cast<FontBlock*>(data)->font);
		return nullptr;
	}

#pragma endregion

#pragma region SharedMusic 实现

	SharedMusic::SharedMusic() noexcept : data(nullptr) {}

	SharedMusic::SharedMusic(const SharedMusic& other) noexcept : data(other.data)
	{
		if (data != nullptr)
		{
			++(static_cast<MusicBlock*>(data)->refCnt);
		}
	}

	SharedMusic::SharedMusic(SharedMusic&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedMusic::SharedMusic(const std::filesystem::path& musicPath) noexcept
	{
		std::string pathStr = musicPath.string();
		std::filesystem::path path(musicPath);
		if (path != musicPath)//有损转换，改为文件读取
		{
			*this = SharedMusic(SharedFile(musicPath));
			return;
		}

		data = new(std::nothrow) MusicBlock(1, LoadMusicStream(pathStr.c_str()));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsMusicValid(static_cast<MusicBlock*>(data)->music))
		{
			delete static_cast<MusicBlock*>(data);
			data = nullptr;
			return;
		}
	}

	SharedMusic::SharedMusic(const SharedFile& fileData) noexcept : data(nullptr)
	{
		if (!fileData.valid())
		{
			return;
		}
		data = new(std::nothrow) MusicBlock(1, ::LoadMusicStreamFromMemory(fileData.fileExtension().string().c_str(), fileData.get(), fileData.size()), fileData);
		if (data == nullptr)
		{
			return;
		}
		if (!::IsMusicValid(static_cast<MusicBlock*>(data)->music))
		{
			delete static_cast<MusicBlock*>(data);
			data = nullptr;
			return;
		}
	}

	SharedMusic& SharedMusic::operator=(const SharedMusic& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		if (other.data != nullptr)
		{
			data = other.data;
			++(static_cast<MusicBlock*>(data)->refCnt);
		}
		return *this;
	}
	
	SharedMusic& SharedMusic::operator=(SharedMusic&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	SharedMusic::~SharedMusic() noexcept
	{
		release();
	}

	void SharedMusic::release() noexcept
	{
		if (data != nullptr)
		{
			--(static_cast<MusicBlock*>(data)->refCnt);
			if (static_cast<MusicBlock*>(data)->refCnt == 0)
			{
				UnloadMusicStream(static_cast<MusicBlock*>(data)->music);
				delete static_cast<MusicBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedMusic::get() noexcept
	{
		if (data) return &(static_cast<MusicBlock*>(data)->music);
		return nullptr;
	}

	const void* SharedMusic::get() const noexcept
	{
		if (data) return &(static_cast<MusicBlock*>(data)->music);
		return nullptr;
	}

#pragma endregion

#pragma region SharedShader 实现

	SharedShader::SharedShader() noexcept : data(nullptr) {}

	SharedShader::SharedShader(const SharedShader& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<ShaderBlock*>(data)->refCnt);
		}
	}

	SharedShader::SharedShader(SharedShader&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedShader::SharedShader(const std::filesystem::path& vs, const std::filesystem::path& fs) noexcept
	{
		std::string vsPath = vs.empty() ? "" : reinterpret_cast<const char*>(vs.u8string().c_str());
		std::string fsPath = fs.empty() ? "" : reinterpret_cast<const char*>(fs.u8string().c_str());
		data = new(std::nothrow) ShaderBlock(1, LoadShader(vsPath.empty() ? nullptr : vsPath.c_str(), fsPath.empty() ? nullptr : fsPath.c_str()));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsShaderValid(static_cast<ShaderBlock*>(data)->shader))
		{
			delete static_cast<ShaderBlock*>(data);
			data = nullptr;
			return;
		}
	}

	SharedShader::SharedShader(const SharedFile& vs, const SharedFile& fs) noexcept
	{
		std::string vsData = vs.valid() ? std::string(reinterpret_cast<const char*>(vs.get()), vs.size()) : "";
		std::string fsData = fs.valid() ? std::string(reinterpret_cast<const char*>(fs.get()), fs.size()) : "";

		data = new(std::nothrow) ShaderBlock(1, LoadShaderFromMemory(vsData.empty() ? nullptr : vsData.c_str(), fsData.empty() ? nullptr : fsData.c_str()));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsShaderValid(static_cast<ShaderBlock*>(data)->shader))
		{
			delete static_cast<ShaderBlock*>(data);
			data = nullptr;
			return;
		}
	}

	SharedShader& SharedShader::operator=(const SharedShader& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(static_cast<ShaderBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedShader& SharedShader::operator=(SharedShader&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	SharedShader::~SharedShader()
	{
		release();
	}

	void SharedShader::release() noexcept
	{
		if (data)
		{
			--(static_cast<ShaderBlock*>(data)->refCnt);
			if (static_cast<ShaderBlock*>(data)->refCnt == 0)
			{
				UnloadShader(static_cast<ShaderBlock*>(data)->shader);
				delete static_cast<ShaderBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedShader::get() noexcept
	{
		if (data) return &(static_cast<ShaderBlock*>(data)->shader);
		return nullptr;
	}

	const void* SharedShader::get() const noexcept
	{
		if (data) return &(static_cast<ShaderBlock*>(data)->shader);
		return nullptr;
	}

#pragma endregion

#pragma region SharedFile 实现

	SharedFile::SharedFile() noexcept : data(nullptr) {}

	SharedFile::SharedFile(std::span<const char> dat, std::filesystem::path path) noexcept : SharedFile(std::span<const std::byte>{reinterpret_cast<const std::byte*>(dat.data()), dat.size_bytes()}, std::move(path)) {}
	SharedFile::SharedFile(std::span<const signed char> dat, std::filesystem::path path) noexcept : SharedFile(std::span<const std::byte>{reinterpret_cast<const std::byte*>(dat.data()), dat.size_bytes()}, std::move(path)) {}
	SharedFile::SharedFile(std::span<const unsigned char> dat, std::filesystem::path path) noexcept : SharedFile(std::span<const std::byte>{reinterpret_cast<const std::byte*>(dat.data()), dat.size_bytes()}, std::move(path)) {}
	SharedFile::SharedFile(std::span<const std::byte> dat, std::filesystem::path path) noexcept : data(nullptr)
	{
		data = ::operator new(sizeof(ControlBlock) + dat.size_bytes(), std::nothrow);
		if (!data) return;
		::new(reinterpret_cast<ControlBlock*>(data)) ControlBlock(1, dat.size_bytes(), std::move(path));
		memcpy(reinterpret_cast<char*>(data) + sizeof(ControlBlock), dat.data(), dat.size_bytes());
	}

	SharedFile::SharedFile(std::filesystem::path path) noexcept : data(nullptr)
	{
		std::ifstream ifs{ path, std::ios::binary };
		if (!ifs) return;

		ifs.seekg(0, std::ios::end);
		const auto endPos = ifs.tellg();
		ifs.seekg(0, std::ios::beg);
		if (!ifs || endPos == std::streampos(-1)) return;

		const auto fileSize = static_cast<size_t>(endPos);
		*this = SharedFile(ifs, fileSize, std::move(path));
	}

	SharedFile::SharedFile(std::istream& is, size_t size, std::filesystem::path path) noexcept : data(nullptr)
	{
		if (size == 0)
		{
			std::vector<uint8_t> dataArr;
			try
			{
				dataArr.insert(dataArr.begin(), std::istreambuf_iterator<char>{is}, std::istreambuf_iterator<char>{});
			}
			catch (...)
			{
				return;
			}
			if (is.fail()) return;
			data = ::operator new(sizeof(ControlBlock) + dataArr.size(), std::nothrow);
			if (!data) return;
			::new(reinterpret_cast<ControlBlock*>(data)) ControlBlock(1, dataArr.size(), std::move(path));
			std::memcpy(reinterpret_cast<char*>(data) + sizeof(ControlBlock), dataArr.data(), dataArr.size());
		}
		else
		{
			data = ::operator new(sizeof(ControlBlock) + size, std::nothrow);
			if (!data) return;
			::new(reinterpret_cast<ControlBlock*>(data)) ControlBlock(1, size, std::move(path));
			try
			{
				is.read(reinterpret_cast<char*>(data) + sizeof(ControlBlock), size);
			}
			catch (...)
			{
				reinterpret_cast<ControlBlock*>(data)->~ControlBlock();
				::operator delete(data);
				data = nullptr;
				return;
			}
			if (!is.good())
			{
				reinterpret_cast<ControlBlock*>(data)->~ControlBlock();
				::operator delete(data);
				data = nullptr;
			}
		}
	}

	SharedFile::SharedFile(const SharedFile& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(reinterpret_cast<ControlBlock*>(data)->ref);
		}
	}

	SharedFile::SharedFile(SharedFile&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedFile& SharedFile::operator=(const SharedFile& other) noexcept
	{
		if (data == other.data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(reinterpret_cast<ControlBlock*>(data)->ref);
		}
		return *this;
	}

	SharedFile& SharedFile::operator=(SharedFile&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	SharedFile::~SharedFile() noexcept
	{
		release();
	}

	SharedFile::Iterator SharedFile::begin() noexcept
	{
		return Iterator(*this);
	}

	SharedFile::Iterator SharedFile::end() noexcept
	{
		return Iterator(*this, size());
	}

	SharedFile::ConstIterator SharedFile::cbegin() const noexcept
	{
		return ConstIterator(*this);
	}

	SharedFile::ConstIterator SharedFile::cend() const noexcept
	{
		return ConstIterator(*this, size());
	}

	void SharedFile::release() noexcept
	{
		if (data)
		{
			auto& ctrl = *reinterpret_cast<ControlBlock*>(data);
			--(ctrl.ref);
			if (ctrl.ref == 0)
			{
				ctrl.~ControlBlock();
				::operator delete(data);
			}
			data = nullptr;
		}
	}

#pragma endregion

#pragma region SharedFile::Iterator 实现
	SharedFile::Iterator::Iterator() : fileSize(0), offset(0), pointer(nullptr) {}

	SharedFile::Iterator::Iterator(size_t size, unsigned char* data, int64_t offset)
		: fileSize(size), pointer(data + offset), offset(offset) {
	}

	SharedFile::Iterator::Iterator(SharedFile& file)
		: fileSize(reinterpret_cast<SharedFile::ControlBlock*>(file.data)->dataSize), pointer(reinterpret_cast<unsigned char*>(file.data) + sizeof(SharedFile::ControlBlock)), offset(0) {
	}

	SharedFile::Iterator::Iterator(SharedFile& file, int64_t offset)
		: fileSize(reinterpret_cast<SharedFile::ControlBlock*>(file.data)->dataSize), pointer(reinterpret_cast<unsigned char*>(file.data) + sizeof(SharedFile::ControlBlock) + offset), offset(offset) {
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

#pragma endregion

#pragma region SharedFile::ConstIterator 实现
	
	SharedFile::ConstIterator::ConstIterator() = default;
	SharedFile::ConstIterator::ConstIterator(size_t size, unsigned char* data, int64_t offset) : it(size, data, offset) {}
	SharedFile::ConstIterator::ConstIterator(const SharedFile& file) : it(const_cast<SharedFile&>(file)) {}
	SharedFile::ConstIterator::ConstIterator(const SharedFile& file, int64_t offset) : it(const_cast<SharedFile&>(file), offset) {}

	SharedFile::ConstIterator::ConstIterator(const ConstIterator& other) : it(other.it) {}
	SharedFile::ConstIterator::ConstIterator(ConstIterator&& other) noexcept : it(std::move(other.it)) {}

	SharedFile::ConstIterator::ConstIterator(const Iterator& other) : it(other) {};
	SharedFile::ConstIterator::ConstIterator(Iterator&& other) noexcept : it(std::move(other)) {}

	SharedFile::ConstIterator& SharedFile::ConstIterator::operator=(const SharedFile::ConstIterator& other) noexcept { it = other.it; return *this; }
	SharedFile::ConstIterator& SharedFile::ConstIterator::operator=(SharedFile::ConstIterator&& other) noexcept { it = std::move(other.it); return *this; };

	SharedFile::ConstIterator& SharedFile::ConstIterator::operator++() noexcept { ++it; return *this; }
	SharedFile::ConstIterator SharedFile::ConstIterator::operator++(int) noexcept { auto r = *this; ++it; return r; }
	SharedFile::ConstIterator& SharedFile::ConstIterator::operator--() noexcept { --it; return *this; }
	SharedFile::ConstIterator SharedFile::ConstIterator::operator--(int) noexcept { auto r = *this; --it; return r; }

	const unsigned char& SharedFile::ConstIterator::operator*() const noexcept { return *it; }
	const unsigned char& SharedFile::ConstIterator::operator[](int64_t _offset) const noexcept { return it[_offset]; }

	bool SharedFile::ConstIterator::eof() const noexcept { return it.eof(); }
	bool SharedFile::ConstIterator::valid() const noexcept { return it.valid(); }
	void SharedFile::ConstIterator::reset() noexcept { it.reset(); }
	SharedFile::ConstIterator::operator bool() const noexcept { return static_cast<bool>(it); }

	SharedFile::ConstIterator& SharedFile::ConstIterator::operator+=(int64_t _offset) noexcept { it += _offset; return *this; }
	SharedFile::ConstIterator& SharedFile::ConstIterator::operator-=(int64_t _offset) noexcept { it -= _offset; return *this; }
	SharedFile::ConstIterator SharedFile::ConstIterator::operator+(int64_t _offset) const noexcept { auto r = *this; return r += _offset; }
	SharedFile::ConstIterator SharedFile::ConstIterator::operator-(int64_t _offset) const noexcept { auto r = *this; return r -= _offset; }
	int64_t SharedFile::ConstIterator::operator-(const SharedFile::ConstIterator& other) const noexcept { return it - other.it; }

	bool SharedFile::ConstIterator::operator==(const SharedFile::ConstIterator& other) const noexcept { return it == other.it; }
	bool SharedFile::ConstIterator::operator>(const SharedFile::ConstIterator& other) const noexcept { return it > other.it; }
	bool SharedFile::ConstIterator::operator>=(const SharedFile::ConstIterator& other) const noexcept { return it >= other.it; }
	bool SharedFile::ConstIterator::operator<(const SharedFile::ConstIterator& other) const noexcept { return it < other.it; }
	bool SharedFile::ConstIterator::operator<=(const SharedFile::ConstIterator& other) const noexcept { return it <= other.it; }
	bool SharedFile::ConstIterator::operator!=(const SharedFile::ConstIterator& other) const noexcept { return it != other.it; }

	size_t SharedFile::ConstIterator::size() const noexcept { return it.size(); }
	size_t SharedFile::ConstIterator::remaining() const noexcept { return it.remaining(); }
	int64_t SharedFile::ConstIterator::position() const noexcept { return it.position(); }
	const unsigned char* SharedFile::ConstIterator::get() const noexcept { return it.get(); }

#pragma endregion

#pragma region SharedRenderTexture 实现

	SharedRenderTexture::SharedRenderTexture() noexcept : data(nullptr) {}

	SharedRenderTexture::SharedRenderTexture(const SharedRenderTexture& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<RenderTextureBlock*>(data)->refCnt);
		}
	}

	SharedRenderTexture::SharedRenderTexture(SharedRenderTexture&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedRenderTexture::SharedRenderTexture(int x, int y) noexcept
	{
		data = new(std::nothrow) RenderTextureBlock(1, LoadRenderTexture(x, y));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsRenderTextureValid(static_cast<RenderTextureBlock*>(data)->renderTexture))
		{
			delete static_cast<RenderTextureBlock*>(data);
			data = nullptr;
		}
	}

	SharedRenderTexture& SharedRenderTexture::operator=(const SharedRenderTexture& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data != nullptr)
		{
			++(static_cast<RenderTextureBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedRenderTexture& SharedRenderTexture::operator=(SharedRenderTexture&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	SharedRenderTexture::~SharedRenderTexture()
	{
		release();
	}

	int SharedRenderTexture::width() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->renderTexture.texture.width;
		return 0;
	}

	int SharedRenderTexture::height() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->renderTexture.texture.height;
		return 0;
	}

	Vec2 SharedRenderTexture::size() const noexcept
	{
		return Vec2{ static_cast<float>(width()), static_cast<float>(height()) };
	}

	void SharedRenderTexture::release() noexcept
	{
		if (data)
		{
			--(static_cast<RenderTextureBlock*>(data)->refCnt);
			if (static_cast<RenderTextureBlock*>(data)->refCnt == 0)
			{
				RenderTexture& rt = static_cast<RenderTextureBlock*>(data)->renderTexture;
				rt.depth = {};
				rt.texture = {};
				::UnloadRenderTexture(rt);
				delete static_cast<RenderTextureBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedRenderTexture::get() noexcept
	{
		if (data) return &(static_cast<RenderTextureBlock*>(data)->renderTexture);
		return nullptr;
	}

	const void* SharedRenderTexture::get() const noexcept
	{
		if (data) return &(static_cast<RenderTextureBlock*>(data)->renderTexture);
		return nullptr;
	}

	SharedTexture& SharedRenderTexture::texture() noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->texture;
		static SharedTexture empty;
		return empty;
	}
	
	const SharedTexture& SharedRenderTexture::texture() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->texture;
		static SharedTexture empty;
		return empty;
	}

	SharedTexture& SharedRenderTexture::depth() noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->depth;
		static SharedTexture empty;
		return empty;
	}

	const SharedTexture& SharedRenderTexture::depth() const noexcept
	{
		if (data) return static_cast<RenderTextureBlock*>(data)->depth;
		static SharedTexture empty;
		return empty;
	}

#pragma endregion

#pragma region SharedSound 实现
	SharedSound::SharedSound() noexcept : data(nullptr) {}

	SharedSound::SharedSound(const SharedSound& other) noexcept : data(other.data)
	{
		if (data)
		{
			++(static_cast<SoundBlock*>(data)->refCnt);
		}
	}

	SharedSound::SharedSound(SharedSound&& other) noexcept : data(other.data)
	{
		other.data = nullptr;
	}

	SharedSound::SharedSound(const std::filesystem::path& soundPath) noexcept
	{
		data = new(std::nothrow) SoundBlock(1, LoadSound(reinterpret_cast<const char*>(soundPath.u8string().c_str())));
		if (data == nullptr)
		{
			return;
		}
		if (!::IsSoundValid(static_cast<SoundBlock*>(data)->sound))
		{
			delete static_cast<SoundBlock*>(data);
			data = nullptr;
		}
	}

	SharedSound& SharedSound::operator=(const SharedSound& other) noexcept
	{
		if (other.data == data)
		{
			return *this;
		}
		release();
		data = other.data;
		if (data)
		{
			++(static_cast<SoundBlock*>(data)->refCnt);
		}
		return *this;
	}

	SharedSound& SharedSound::operator=(SharedSound&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	SharedSound::~SharedSound() noexcept
	{
		release();
	}

	void SharedSound::release() noexcept
	{
		if (data)
		{
			--(static_cast<SoundBlock*>(data)->refCnt);
			if (static_cast<SoundBlock*>(data)->refCnt == 0)
			{
				UnloadSound(static_cast<SoundBlock*>(data)->sound);
				delete static_cast<SoundBlock*>(data);
			}
			data = nullptr;
		}
	}

	void* SharedSound::get() noexcept
	{
		if (data) return &(static_cast<SoundBlock*>(data)->sound);
		return nullptr;
	}

	const void* SharedSound::get() const noexcept
	{
		if (data) return &(static_cast<SoundBlock*>(data)->sound);
		return nullptr;
	}

#pragma endregion

#pragma region Image序列化支持

	SharedImage CreateImageFromData(const std::vector<char>& data)
	{
		::Image img = {};
		BufferIS buffer;
		const char* p = data.data();

		buffer.set(p, sizeof(::Image::width));
		utils::Deserialize(buffer, img.width);
		p += sizeof(::Image::width);

		buffer.set(p, sizeof(::Image::height));
		utils::Deserialize(buffer, img.height);
		p += sizeof(::Image::height);

		buffer.set(p, sizeof(::Image::mipmaps));
		utils::Deserialize(buffer, img.mipmaps);
		p += sizeof(::Image::mipmaps);

		buffer.set(p, sizeof(::Image::format));
		utils::Deserialize(buffer, img.format);
		p += sizeof(::Image::format);

		uint64_t size = static_cast<uint64_t>(GetPixelDataSize(img.width, img.height, img.format));
		img.data = RL_MALLOC(size);
		if (!img.data) return SharedImage{};
		memcpy(img.data, p, size);

		ResourceCreator creator;
		return creator.CreateImage(img);
	}

	ImageDataIterator& ImageDataIterator::operator++()
	{
		Image* img = static_cast<::Image*>(this->img.get());
		switch (index)
		{
		case 0:
		{
			uint64_t dataSize = GetPixelDataSize(img->width, img->height, img->format);
			dataSize += sizeof(img->width) + sizeof(img->height) + sizeof(img->mipmaps) + sizeof(img->format);
			utils::Serialize(buffer, dataSize);
			data = buffer.span();
			break;
		}
		case 1:
			utils::Serialize(buffer, img->width);
			data = buffer.span();
			break;

		case 2:
			utils::Serialize(buffer, img->height);
			data = buffer.span();
			break;

		case 3:
			utils::Serialize(buffer, img->mipmaps);
			data = buffer.span();
			break;

		case 4:
			utils::Serialize(buffer, img->format);
			data = buffer.span();
			break;

		case 5:
			data = std::span<char>(static_cast<char*>(img->data), GetPixelDataSize(img->width, img->height, img->format));
			break;

		default:
			index = 7;
			return *this;
		}
		++index;
		return *this;
	}

	ImageDataIterator ImageDataIterator::operator++(int)
	{
		ImageDataIterator tmp = *this;
		++(*this);
		return tmp;
	}

	bool ImageDataIterator::operator==(const ImageDataIterator& other) const
	{
		return index == other.index && img.get() == other.img.get();
	}

	bool ImageDataIterator::operator!=(const ImageDataIterator& other) const
	{
		return !(*this == other);
	}

	ImageDataIterator ImageDataRange::begin() const
	{
		ImageDataIterator it;
		it.img = img;
		it.index = 0;
		it.data = std::span<char>();
		++it;
		return it;
	}

	ImageDataIterator ImageDataRange::end() const
	{
		ImageDataIterator it;
		it.img = img;
		it.index = 7;
		it.data = std::span<char>();
		return it;
	}

#pragma endregion
}