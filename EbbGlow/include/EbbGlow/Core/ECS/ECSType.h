#pragma once

#include <cstdint>

namespace ebbglow::core
{
	using entity = uint64_t;

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

	class DrawBase
	{
	public:
		DrawBase() = default;
		virtual ~DrawBase() = default;
		virtual void draw() = 0;
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