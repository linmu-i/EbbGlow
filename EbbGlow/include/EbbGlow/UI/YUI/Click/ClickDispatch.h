#pragma once
#include <functional>

#include <EbbGlow/Core/World.h>

namespace ebbglow::ui::yui
{
	namespace ClickPriority
	{
		using Type = uint16_t;
		constexpr Type MaxValue = std::numeric_limits<Type>::max();
		constexpr Type MinValue = std::numeric_limits<Type>::min();
	}
	using ClickPriorityT = ClickPriority::Type;
	struct ClickRequest
	{
		ClickPriorityT priority;
		core::entity entity;
		std::function<void(core::World2D&, core::entity)> callback;
	};

	class ClickDispatchSystem : public core::SystemBase
	{
	private:
		std::unique_ptr<std::mutex> mtx;
		std::vector<ClickRequest> requests;
		
		core::World2D* world = nullptr;

	public:
		ClickDispatchSystem(core::World2D& world) : world(&world), mtx(std::make_unique<std::mutex>()) {};
		void update() override;
		void addRequest(ClickRequest request);
	};

	void AddClickRequest(core::World2D& world, ClickRequest request);

	void ApplyClickDispatch(core::World2D& world);
}