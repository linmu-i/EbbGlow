#pragma once

#include <EbbGlow/Core/World.h>

#include <EbbGlow/UI/YUI/UILayout/Layout.h>

namespace ebbglow::ui::yui::layout
{
	struct ChildData
	{
		core::entity childId;
		Vec2 size;
		UILayout layout;
		std::function<void(ChildData&)> callback;
	};

	struct Context
	{
		core::entity parentId;
		std::vector<ChildData> children;
	};

	class ContextStack
	{
	private:
		std::vector<Context> stack;
		core::World2D* world;

		std::vector<core::entity> idList;
		size_t currentIndex = 0;

		core::DrawPriorityT beginDrawPriority = 0;

	public:
		ContextStack(core::World2D* world, std::vector<core::entity> idList, core::DrawPriorityT beginDrawPriority) :
			world(world), idList(std::move(idList)), beginDrawPriority(beginDrawPriority) {}
		void pushContext(core::entity parentId)
		{
			stack.push_back({ parentId, {} });
		}
		void popContext()
		{
			if (!stack.empty())
			{
				stack.pop_back();
			}
		}
		void addChild(core::entity childId, Vec2 size, UILayout layout, std::function<void(ChildData&)> callback)
		{
			if (!stack.empty())
			{
				stack.back().children.push_back({ childId, size, layout, callback });
			}
		}
		core::entity getCurrentParent() const
		{
			if (!stack.empty())
			{
				return stack.back().parentId;
			}
			return core::entity(); // Return an invalid entity if the stack is empty
		}
		std::vector<ChildData>& getCurrentChildren()
		{
			if (!stack.empty())
			{
				return stack.back().children;
			}
			static std::vector<ChildData> emptyChildren;
			return emptyChildren; // Return an empty vector if the stack is empty
		}

		core::entity getNextId(bool createInvalid = false)
		{
			if (currentIndex < idList.size())
			{
				return idList[currentIndex++];
			}
			else
			{
				core::entity newId = createInvalid ? core::entity() : world->getEntityManager()->getId();
				idList.push_back(newId);
				currentIndex++;
				return newId;
			}
		}

		core::World2D& getWorld() const
		{
			return *world;
		}

		std::vector<core::entity>& getIdList()
		{
			return idList;
		}

		size_t depth()
		{
			return stack.size();
		}

		core::DrawPriorityT getBeginDrawPriority() const
		{
			return beginDrawPriority;
		}
	};

	ContextStack& GetContextStack();

	class ContextStackGuard
	{
	public:
		ContextStackGuard(core::entity parentId)
		{
			GetContextStack().pushContext(parentId);
		}
		~ContextStackGuard()
		{
			GetContextStack().popContext();
		}
	};
	void InitContextStack(core::World2D& world, std::vector<core::entity> idList = {}, core::DrawPriorityT beginDrawPriority = 0);
}