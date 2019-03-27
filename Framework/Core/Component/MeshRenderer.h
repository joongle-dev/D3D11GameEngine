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

	void SetMesh(class Mesh* mesh) { m_mesh = mesh; }
	class Mesh* GetMesh() const { return m_mesh; }

	void SetMaterial(class Material* material) { m_material = material; }
	class Material* GetMaterial() const { return m_material; }
	class Transform* GetTransform() const { return m_transform; }

private:
	class Transform* m_transform;

	class Material* m_material;
	class Mesh* m_mesh;
};