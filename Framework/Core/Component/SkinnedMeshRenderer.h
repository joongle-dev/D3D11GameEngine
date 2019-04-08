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

	class Transform* GetTransform() const { return mTransform; }

	void SetRootTransform(class Transform* transform) { mRootTransform = transform; }

private:
	class Transform* mRootTransform;
	class Transform* mTransform;
	class Material*  mMaterial;
	class Mesh*      mMesh;
};