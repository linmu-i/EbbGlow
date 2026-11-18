#include <cstring>
#include <new>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

#include <raylib.h>
#include <EbbGlow/Resources/SharedFile.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
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
}
