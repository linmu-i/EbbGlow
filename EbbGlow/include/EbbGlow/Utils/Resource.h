#pragma once

#include <string>
#include <span>
#include <filesystem>

#include <EbbGlow/Utils/Types.h>
#include <EbbGlow/Utils/Serialization.h>

namespace ebbglow::resource
{
	class SharedFile
	{
	private:
		struct ControlBlock
		{
			std::atomic<size_t> ref;
			size_t dataSize;
			std::filesystem::path path;
			ControlBlock(size_t ref, size_t dataSize, const std::filesystem::path& path) : ref(ref), dataSize(dataSize), path(path) {}
		};

		void* data;

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
			Iterator(SharedFile& file);
			Iterator(SharedFile& file, int64_t offset);

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

		class ConstIterator
		{
		private:
			Iterator it;

		public:
			ConstIterator();
			ConstIterator(size_t size, unsigned char* data, int64_t offset = 0);
			ConstIterator(const SharedFile& file);
			ConstIterator(const SharedFile& file, int64_t offset);

			ConstIterator(const ConstIterator& other);
			ConstIterator(ConstIterator&& other) noexcept;

			ConstIterator(const Iterator& other);
			ConstIterator(Iterator&& other) noexcept;

			ConstIterator& operator=(const ConstIterator& other) noexcept;
			ConstIterator& operator=(ConstIterator&& other) noexcept;

			ConstIterator& operator++() noexcept;
			ConstIterator operator++(int) noexcept;
			ConstIterator& operator--() noexcept;
			ConstIterator operator--(int) noexcept;

			const unsigned char& operator*() const noexcept;
			const unsigned char& operator[](int64_t _offset) const noexcept;

			bool eof() const noexcept;
			bool valid() const noexcept;
			void reset() noexcept;
			explicit operator bool() const noexcept;

			ConstIterator& operator+=(int64_t _offset) noexcept;
			ConstIterator& operator-=(int64_t _offset) noexcept;
			ConstIterator operator+(int64_t _offset) const noexcept;
			ConstIterator operator-(int64_t _offset) const noexcept;
			int64_t operator-(const ConstIterator& other) const noexcept;

			bool operator==(const ConstIterator& other) const noexcept;
			bool operator>(const ConstIterator& other) const noexcept;
			bool operator>=(const ConstIterator& other) const noexcept;
			bool operator<(const ConstIterator& other) const noexcept;
			bool operator<=(const ConstIterator& other) const noexcept;
			bool operator!=(const ConstIterator& other) const noexcept;

			size_t size() const noexcept;
			size_t remaining() const noexcept;
			int64_t position() const noexcept;
			const unsigned char* get() const noexcept;
		};

		SharedFile() noexcept;
		SharedFile(std::span<const char>, std::filesystem::path path = "") noexcept;
		SharedFile(std::span<const signed char>, std::filesystem::path path = "") noexcept;
		SharedFile(std::span<const unsigned char>, std::filesystem::path path = "") noexcept;
		SharedFile(std::span<const std::byte>, std::filesystem::path path = "") noexcept;
		SharedFile(std::filesystem::path path) noexcept;
		SharedFile(std::istream& is, size_t size, std::filesystem::path path = "") noexcept;
		SharedFile(const SharedFile& other) noexcept;
		SharedFile(SharedFile&& other) noexcept;

		SharedFile& operator=(const SharedFile& other) noexcept;
		SharedFile& operator=(SharedFile&& other) noexcept;
		~SharedFile() noexcept;

		const unsigned char* get() const noexcept { return reinterpret_cast<unsigned char*>(data) + sizeof(ControlBlock); }
		size_t size() const noexcept { return reinterpret_cast<ControlBlock*>(data)->dataSize; }
		std::filesystem::path fileName() const noexcept { return data == nullptr ? "" : reinterpret_cast<ControlBlock*>(data)->path.filename(); }
		std::filesystem::path filePath() const noexcept { return data == nullptr ? "" : reinterpret_cast<ControlBlock*>(data)->path; }
		std::filesystem::path fileExtension() const noexcept { return data == nullptr ? "" : reinterpret_cast<ControlBlock*>(data)->path.extension(); }

		operator bool() const noexcept { return data; }
		bool valid() const noexcept { return data; }

		Iterator begin() noexcept;
		Iterator end() noexcept;

		ConstIterator cbegin() const noexcept;
		ConstIterator cend() const noexcept;

		void release() noexcept;
	};

	class SharedImage
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedImage() noexcept;
		SharedImage(const std::filesystem::path& path) noexcept;
		SharedImage(const SharedFile& file) noexcept;
		SharedImage(const SharedImage& other) noexcept;
		SharedImage(SharedImage&& other) noexcept;
		~SharedImage() noexcept;

		SharedImage& operator=(const SharedImage& other) noexcept;
		SharedImage& operator=(SharedImage&& other) noexcept;

		explicit operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		int width() const noexcept;
		int height() const noexcept;

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
	};

	class SharedTexture
	{
	private:
		void* data;

		friend class ResourceCreator;

	public:
		SharedTexture() noexcept;
		SharedTexture(const std::filesystem::path& path) noexcept;
		SharedTexture(const SharedImage& img) noexcept;
		SharedTexture(const SharedTexture& other) noexcept;
		SharedTexture(SharedTexture&& other) noexcept;
		~SharedTexture() noexcept;

		SharedTexture& operator=(const SharedTexture& other) noexcept;
		SharedTexture& operator=(SharedTexture&& other) noexcept;

		explicit operator bool() const noexcept { return data != nullptr; }
		bool valid() const noexcept { return data != nullptr; }

		int width() const noexcept;
		int height() const noexcept;
		Vec2 size() const noexcept;

		void* get() noexcept;
		const void* get() const noexcept;

		void release() noexcept;
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



	class BufferOS
	{
	private:
		std::vector<char> buffer;
		uint64_t dataSize_;
	public:
		BufferOS(uint64_t size) : buffer(size), dataSize_(0) {}
		bool write(const char* data, uint64_t size)
		{
			if (size > buffer.size())
			{
				dataSize_ = 0;
				return false;
			}
			dataSize_ = size;
			std::copy(data, data + size, buffer.data());
			return true;
		}
		const char* data() const
		{
			return buffer.data();
		}

		uint64_t dataSize() const
		{
			return dataSize_;
		}

		std::span<char> span()
		{
			return std::span<char>(buffer.data(), dataSize_);
		}
	};

	class BufferIS
	{
	private:
		std::span<const char> buffer;
	public:
		BufferIS() = default;
		void set(const char* data, uint64_t size)
		{
			buffer = std::span<const char>(data, size);
		}
		bool read(char* buf, uint64_t size)
		{
			if (size > buffer.size()) return false;
			memcpy(buf, buffer.data(), size);
			return true;
		}
	};

	SharedImage CreateImageFromData(const std::vector<char>& data);

	struct ImageDataIterator
	{
		uint64_t index = {};
		std::span<char> data;
		SharedImage img;
		BufferOS buffer{ 16 };

		ImageDataIterator& operator++();
		ImageDataIterator operator++(int);
		bool operator==(const ImageDataIterator& other) const;
		bool operator!=(const ImageDataIterator& other) const;
		std::span<char>& operator*() { return data; }
		const std::span<char>& operator*() const { return data; }
	};

	class ImageDataRange
	{
	private:
		SharedImage img;

	public:
		ImageDataRange(const SharedImage& image) : img(image) {}
		ImageDataIterator begin() const;
		ImageDataIterator end() const;
	};

	template<utils::OutStream OS>
	bool Serialize(OS& os, const SharedImage& image)
	{
		if (!image.valid()) return false;
		ImageDataRange range(image);
		for (auto& data : range)
		{
			os.write(data.data(), data.size());
		}
	}

	template<utils::InStream IS>
	bool Deserialize(IS& is, SharedImage& image)
	{
		std::vector<char> data = {};
		if (!utils::Deserialize(is, data)) return false;
		if (data.empty()) return false;
		image = CreateImageFromData(data);
		return true;
	}
}

namespace ebbglow
{
	namespace rsc = resource;
}