#pragma once

#include <array>

#include <EbbGlow/Core/ECS.h>
#include <EbbGlow/Core/Message.h>

namespace ebbglow::core
{
	using Layers = std::array<std::vector<std::unique_ptr<DrawBase>>, 16>;
	using Layer = std::vector<std::unique_ptr<DrawBase>>;

	class AddBufferBase
	{
	protected:
		ComponentPoolBase* pool0;
		ComponentPoolBase* pool1;
	public:
		AddBufferBase(ComponentPoolBase* pool0, ComponentPoolBase* pool1) : pool0(pool0), pool1(pool1) {}
		virtual ~AddBufferBase() = default;
		virtual void addToPool() = 0;
	};

	template<typename T>
	class AddBuffer : public AddBufferBase
	{
	private:
		std::vector<std::pair<entity, T>> buffer;

	public:
		AddBuffer(ComponentPoolBase* pool0, ComponentPoolBase* pool1) : AddBufferBase(pool0, pool1) {}
		void addBuffer(entity id, T&& data)
		{
			buffer.emplace_back(id, std::forward<T>(data));
		}

		void addToPool() override
		{
			ComponentPool<T>* p0 = static_cast<ComponentPool<T>*>(pool0);
			ComponentPool<T>* p1 = static_cast<ComponentPool<T>*>(pool1);
			for (auto& b : buffer)
			{
				p0->add(b.first, b.second);
				p1->add(b.first, b.second);
			}
			buffer.clear();
		}
	};
}