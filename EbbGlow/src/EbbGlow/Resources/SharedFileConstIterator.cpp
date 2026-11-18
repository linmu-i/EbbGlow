#include <cstdint>
#include <utility>

#include <EbbGlow/Resources/SharedFile.h>

namespace ebbglow::resource
{
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
}
