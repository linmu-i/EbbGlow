#pragma once

#include <EbbGlow/Core/World.h>

namespace ebbglow::core
{
	template<typename ComT, typename SysT>
	class SubSystem
	{
	private:
		ComponentPool<ComT> pool0;
		ComponentPool<ComT> pool1;
		DoubleComs<ComT> doubleComs;
		SysT system;

		std::vector<entity> waitDelete;
		AddBuffer<ComT> waitAdd;

		IdManager idManager;

	public:
		template<typename... SysArg>
		SubSystem(SysArg&&... sysArg) : doubleComs(&pool0, &pool1, false),
			system(&doubleComs, std::forward<SysArg>(sysArg)...), waitAdd(&pool0, &pool1) {}

		void update()
		{
			for (auto& id : waitDelete)
			{
				pool0.remove(id);
				pool1.remove(id);
			}
			waitDelete.clear();
			waitAdd.addToPool();

			doubleComs.sync();
			system.update();
			doubleComs.swap();
		}
	};
}