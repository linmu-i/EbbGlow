#include <EbbGlow/UI/YUI/Transform.h>

namespace ebbglow::ui::yui
{
	std::vector<Transform> GetTransforms(core::ComponentPool<TransformCom>* pool, core::entity id)
	{
		if (!pool) return {};
		auto* transformCom = pool->get(id);
		if (!transformCom) return {};
		std::vector<Transform> result;
		result.reserve(5);
		auto actId = id;

		while (actId != core::InvalidEntity)
		{
			auto* actTransform = pool->get(actId);
			if (!actTransform) break;
			result.push_back(actTransform->transform);
			actId = actTransform->parent;
		}
		std::reverse(result.begin(), result.end());
		return result;
	}

	std::vector<Transform> GetTransforms(core::World2D& world, core::entity id)
	{
		return GetTransforms(world.getDoubleBuffer<TransformCom>()->active(), id);
	}

	Transform GetFinalTransform(std::span<const Transform> trans)
	{
		if (trans.empty()) return Transform{};
		Vec2 pivot = trans[0].pivot + trans[0].position;
		float rot = trans[0].rotation;
		float scl = trans[0].scale;

		for (uint64_t i = 0; i < trans.size() - 1; ++i)
		{
			auto& prev = trans[i];
			auto& next = trans[i + 1];
			Vec2 direction = (next.position + next.pivot) - prev.pivot;
			Vec2 result = direction.rotated(rot) * scl;
			pivot += result;
			rot += next.rotation;
			scl *= next.scale;
		}
		Vec2 finalPosition = pivot - trans.back().pivot;
		return Transform{ finalPosition, trans.back().pivot, rot, scl };
	}

	void TransformAttachTo(TransformCom& child, core::entity parentId)
	{
		child.parent = parentId;
	}

	void TransformAttachTo(core::World2D& world, core::entity child, core::entity parent)
	{
		auto* pool = world.getDoubleBuffer<TransformCom>()->inactive();
		if (!pool) return;
		auto* childTransform = pool->get(child);
		if (!childTransform)
		{
			childTransform = world.getWaitAdd<TransformCom>(child);
		}
		if (!childTransform) return;

		childTransform->parent = parent;
	}
}