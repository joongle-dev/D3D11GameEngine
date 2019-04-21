#pragma once
#include "Component.h"

class SkinnedMeshRenderer final : public Component<SkinnedMeshRenderer>
{
public:
	SkinnedMeshRenderer(class Context* context, class GameObject* owner);
	~SkinnedMeshRenderer() = default;

	void Update() override;

	void SetMesh(class Mesh* mesh) { mMesh = mesh; }
	class Mesh* GetMesh() const { return mMesh; }

	void SetMaterial(class Material* material) { mMaterial = material; }
	class Material* GetMaterial() const { return mMaterial; }

	void SetRootTransform(class Transform* transform) { mRootTransform = transform; }
	class Transform* GetRootTransform() const { return mRootTransform; }

private:
	class Transform* mRootTransform;

	class Material*  mMaterial;
	class Mesh*      mMesh;
};