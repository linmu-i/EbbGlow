#include <EbbGlow/Core/World/World2D.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::core
{
	void World2D::draw()
	{
		gfx::ClearBackground(colors::Black);

		BeginMode2D(camera);
		unitsLayer.sort();
		unitsLayer.draw();
		EndMode2D();

		uiLayer.sort();
		uiLayer.draw();

		uiLayer.clear();
		unitsLayer.clear();
	}

	void World2D::update()
	{
		for (size_t i = 0; i < waitDelete.size(); ++i)
		{
			entity id = waitDelete[i];
			for (auto& ps : comPools0)
			{
				ps.second->remove(id);
			}
			for (auto& ps : comPools1)
			{
				ps.second->remove(id);
			}
		}
		for (auto& b : doubleBuffered)
		{
			b.second->sync();
		}
		for (auto& wa : waitAdd)
		{
			auto it0 = comPools0.find(wa.first);
			auto it1 = comPools1.find(wa.first);
			if (it0 != comPools0.end() && it1 != comPools1.end())
			{
				wa.second->addToPool();
			}
		}
		waitDelete.clear();
		messageManager.sendAll();

		std::vector<std::future<void>> futures;

		for (auto& s : systems)
		{
			if (s.executionPolicy == SystemExecutionPolicies::Detached && threadPool)
			{
				auto systemPtr = s.system.get();
				futures.push_back(threadPool->enqueue([systemPtr]() { systemPtr->update(); }));
			}
			else
			{
				s.system->update();
			}
		}

		for (auto& f : futures)
		{
			f.wait();
		}

		for (auto& b : doubleBuffered)
		{
			b.second->swap();
		}
		messageManager.swap();
		++framesCount;
	}
}