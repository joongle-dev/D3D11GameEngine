#pragma once
#include "Component.h"

enum class ProjectionType : unsigned int
{
	Perspective,
	Orthographic,
};

class Camera final : public Component<Camera>
{
public:
	Camera(class Context* context, class GameObject* owner);
	~Camera() = default;

	void Update() override;

	const DirectX::XMFLOAT4X4 GetViewMatrix();
	const DirectX::XMFLOAT4X4 GetProjectionMatrix();
	
	const float GetNearPlane() const { return m_near; }
	const float GetFarPlane() const { return m_far; }
	const float GetFOV() const { return m_fov; }
	void SetNearPlane(const float np) { m_near = np; }
	void SetFarPlane(const float fp) { m_far = fp; }
	void SetFOV(const float fov) { m_fov = fov; }

	void SetProjectionType(ProjectionType type) { m_type = type; }

	class RenderTarget* GetRenderTarget() const { return m_rendertarget; }
	void SetRenderTarget(class RenderTarget* target) { m_rendertarget = target; }

private:
	class Transform* m_transform;

	class RenderTarget* m_rendertarget;
	ProjectionType m_type;
	float m_near;
	float m_far;
	float m_fov;
};