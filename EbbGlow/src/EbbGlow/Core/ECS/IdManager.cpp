#include <EbbGlow/Core/ECS/IdManager.h>

namespace ebbglow::core
{
	entity IdManager::getId()
	{
		if (inactive.empty())
		{
			return entity(maxIdx++, 0);
		}
		else
		{
			entity tmp = *inactive.begin();
			inactive.erase(inactive.begin());
			return entity(tmp.index(), tmp.version() + 1);
		}
	}

	void IdManager::recycleId(entity id)
	{
		inactive.insert(id);
	}

	size_t IdManager::size() const
	{
		return static_cast<size_t>(maxIdx) - inactive.size();
	}
}
