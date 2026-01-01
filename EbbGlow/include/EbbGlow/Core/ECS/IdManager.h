#pragma once
#include <EbbGlow/Core/ECS/ECSType.h>
#include <vector>

namespace ebbglow::core
{
	class IdManager
	{
	private:
		uint64_t countId = 0;
		std::vector<entity> inactive;
	public:
		uint64_t getId();
		void recycleId(uint64_t id);
		size_t size() const;
	};
}