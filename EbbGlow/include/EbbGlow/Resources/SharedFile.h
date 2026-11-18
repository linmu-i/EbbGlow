#pragma once

#include <filesystem>
#include <span>

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
}