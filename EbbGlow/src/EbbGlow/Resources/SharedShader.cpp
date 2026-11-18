#include <new>
#include <string>

#include <raylib.h>
#include <EbbGlow/Resources/SharedShader.h>
#include "ResourceBlock.h"

namespace ebbglow::resource
{
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
}
