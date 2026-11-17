#pragma once

#include <vector>
#include <span>

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Utils/Math.h>

namespace ebbglow::ui::yui
{
	struct Transform
	{
		Vec2 position;
		Vec2 pivot;
		float rotation = 0.0f;
		float scale = 1.0f;
	};
	struct TransformCom
	{
		Transform transform;
		core::entity parent = core::InvalidEntity;
	};

	std::vector<Transform> GetTransforms(core::ComponentPool<TransformCom>* pool, core::entity id);

	std::vector<Transform> GetTransforms(core::World2D& world, core::entity id);

	Transform GetFinalTransform(std::span<const Transform> trans);

	void TransformAttachTo(TransformCom& child, core::entity parentId);
	void TransformAttachTo(core::World2D& world, core::entity child, core::entity parent);
}