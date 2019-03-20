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

	const Matrix GetViewMatrix();
	const Matrix GetProjectionMatrix();
	
	const float GetNearPlane() { return m_near; }
	const float GetFarPlane() { return m_far; }
	const float GetFOV() { return m_fov * TO_DEG; }
	void SetNearPlane(const float np) { m_near = np; }
	void SetFarPlane(const float fp) { m_far = fp; }
	void SetFOV(const float fov) { m_fov = fov * TO_RAD; }

	void SetProjectionType(ProjectionType type) { m_type = type; }

	class RenderTarget* GetRenderTarget() const { return m_rendertarget; }
	void SetRenderTarget(class RenderTarget* target) { m_rendertarget = target; }

public:
	void InitializeSerializable() override
	{
		RegisterSerializeField("Fov", &Camera::GetFOV, &Camera::SetFOV);
		RegisterSerializeField("Near", &Camera::GetNearPlane, &Camera::SetNearPlane);
		RegisterSerializeField("Far", &Camera::GetFarPlane, &Camera::SetFarPlane);
	}

private:
	class Transform* m_transform;

	class RenderTarget* m_rendertarget;
	ProjectionType m_type;
	float m_near;
	float m_far;
	float m_fov;
};