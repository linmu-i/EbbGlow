#include <cstdint>

#include <EbbGlow/Resources/SharedFile.h>

namespace ebbglow::resource
{
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
}
