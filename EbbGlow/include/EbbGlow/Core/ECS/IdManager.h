#pragma once
#include <EbbGlow/Core/ECS/ECSType.h>
#include <set>

namespace ebbglow::core
{
	class IdManager
	{
	private:
		EntityIndexT maxIdx = 0;
		std::set<entity> inactive;
	public:
		entity getId();
		void recycleId(entity id);
		size_t size() const;
	};
}