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

	class Transform* GetTransform() const { return m_transform; }

private:
	class Transform* m_transform;

	class Texture* m_texture;
	class Mesh* m_mesh;
};