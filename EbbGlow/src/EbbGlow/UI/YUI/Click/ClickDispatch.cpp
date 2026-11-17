#include <EbbGlow/UI/YUI/Click/ClickDispatch.h>

namespace ebbglow::ui::yui
{
	void ClickDispatchSystem::update()
	{
		if (requests.empty()) return;
		auto greatPriority = std::min_element(requests.begin(), requests.end(), [](const ClickRequest& a, const ClickRequest& b) {
			return a.priority < b.priority;
			});
		greatPriority->callback(*world, greatPriority->entity);
		requests.clear();
	}
	void ClickDispatchSystem::addRequest(ClickRequest request)
	{
		std::lock_guard<std::mutex> lock(*mtx);
		requests.push_back(request);
	}
	void AddClickRequest(core::World2D& world, ClickRequest request)
	{
		auto* system = world.getSystem<ClickDispatchSystem>();
		system->addRequest(request);
	}

	void ApplyClickDispatch(core::World2D& world)
	{
		world.addPool<ClickRequest>();
		world.addSystem(ClickDispatchSystem{ world }, ebbglow::core::SystemPriority::Low);
	}
}