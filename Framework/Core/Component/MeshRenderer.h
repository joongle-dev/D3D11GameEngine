#pragma once
#include "Component.h"

class MeshRenderer final : public Component<MeshRenderer>
{
public:
	MeshRenderer(class Context* context, class GameObject* owner);
	~MeshRenderer() = default;

	MeshRenderer(const MeshRenderer& rhs);
	MeshRenderer& operator=(const MeshRenderer&) = delete;

	void Update() override;

	void SetMesh(class Mesh* mesh) { mMesh = mesh; }
	class Mesh* GetMesh() const { return mMesh; }

	void SetMaterial(class Material* material) { mMaterial = material; }
	class Material* GetMaterial() const { return mMaterial; }

	class Transform* GetTransform() const { return mTransform; }

private:
	class Transform* mTransform;
	class Material*  mMaterial;
	class Mesh*      mMesh;
};