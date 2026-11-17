#pragma once

#include <cstdint>

namespace ebbglow::core
{
	using EntityIndexT = uint32_t;
	using EntityVersionT = uint32_t;

	constexpr EntityIndexT InvalidIndex = 0xffffffffu;

	class entity
	{
	private:
		uint32_t index_;
		uint32_t version_;

	public:
		constexpr entity() : index_(InvalidIndex), version_(0xffffffffu) {}
		constexpr entity(uint32_t index, uint32_t version) : index_(index), version_(version) {}
		constexpr uint32_t index() const { return index_; }
		constexpr uint32_t version() const { return version_; }
		constexpr bool operator==(const entity& other) const
		{
			return index_ == other.index_ && version_ == other.version_;
		}
		constexpr bool operator!=(const entity& other) const
		{
			return !(*this == other);
		}
		constexpr bool operator<(const entity& other) const
		{
			return index_ < other.index_;
		}
		constexpr bool operator>(const entity& other) const
		{
			return index_ > other.index_;
		}
	};
	
	constexpr entity InvalidEntity = entity(0xffffffffu, 0xffffffffu);

	template<typename T>
	class DoubleBuffered;

	template<typename T>
	class ComponentPool;

	template<typename T>
	using DoubleComs = core::DoubleBuffered<core::ComponentPool<T>*>;

	class SystemBase
	{
	public:
		SystemBase() = default;
		virtual ~SystemBase() = default;
		virtual void update() = 0;
	};

	struct DrawableFlags
	{
		uint64_t flags = 0ull;

		constexpr DrawableFlags() = default;
		constexpr DrawableFlags(uint64_t flags) : flags(flags) {}

		operator uint64_t() const { return flags; }

		constexpr bool hasFlag(uint64_t flag) const { return flags & flag; }
		constexpr void setFlag(uint64_t flag) { flags |= flag; }
		constexpr void removeFlag(uint64_t flag) { flags &= (~flag); }
		constexpr void clearFlags() { flags = None; }

		static constexpr uint64_t None = 0ull;
		static constexpr uint64_t ExcludeRecursive = 1ull;
	};

	class DrawBase
	{
	public:
		DrawBase() = default;
		virtual ~DrawBase() = default;
		virtual void draw() = 0;
		virtual DrawableFlags getFlags() { return { DrawableFlags::None }; }
	};

	class DoubleBufferedBase
	{
	public:
		virtual void swap() = 0;
		virtual void sync() = 0;
		DoubleBufferedBase() = default;
		virtual ~DoubleBufferedBase() = default;
	};

	template<typename T>
	class DoubleBuffered : public DoubleBufferedBase
	{
	private:
		T data0;
		T data1;
		bool state;
	public:
		DoubleBuffered(T buffer0, T buffer1, bool state) : data0(buffer0), data1(buffer1), state(state) {}
		const T& active() const
		{
			return state ? data0 : data1;
		}
		T& active()
		{
			return state ? data0 : data1;
		}
		const T& inactive() const
		{
			return state ? data1 : data0;
		}
		T& inactive()
		{
			return state ? data1 : data0;
		}
		void swap() noexcept override
		{
			state = !state;
		}
		void sync() override
		{
			inactive()->sync(active());
		}
	};
}