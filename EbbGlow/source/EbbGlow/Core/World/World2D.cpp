#include <EbbGlow/Core/World/World2D.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Resource.h>

namespace ebbglow::core
{
	void World2D::draw()
	{
		BeginTextureMode(activeRenderTexture());
		gfx::ClearBackground(colors::Black);
		BeginMode2D(camera);
		for (int i = 0; i < 16; i++)
		{
			for (auto& d : unitsLayer[i])
			{
				d->draw();
			}
		}
		EndMode2D();
		for (int i = 0; i < 16; i++)
		{
			for (auto& d : uiLayer[i])
			{
				d->draw();
			}
		}
		for (int i = 0; i < 16; i++)
		{
			uiLayer[i].clear();
			unitsLayer[i].clear();
		}
		EndTextureMode();
		gfx::DrawTextureRegion(activeRenderTexture(),
			Rect{ 0, 0, static_cast<float>(activeRenderTexture().width()), static_cast<float>(-activeRenderTexture().height()) },
			Vec2{ 0, 0 },
			colors::White);
	}

	void World2D::update()
	{
		for (auto i : waitDelete)
		{
			for (auto& ps : comPools0)
			{
				ps.second->remove(i);
			}
			for (auto& ps : comPools1)
			{
				ps.second->remove(i);
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
		for (auto& s : systems)
		{
			s->update();
		}
		for (auto& b : doubleBuffered)
		{
			b.second->swap();
		}
		messageManager.swap();
		++framesCount;
	}
}