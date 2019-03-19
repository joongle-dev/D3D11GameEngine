#include "Framework.h"
#include "Camera.h"


using namespace DirectX;

Camera::Camera(Context * context, GameObject* owner) :
	Component<Camera>(context, owner),
	m_near(0.1f), 
	m_far(1000.0f), 
	m_fov(XM_PIDIV2), 
	m_type(ProjectionType::Perspective), 
	m_rendertarget(nullptr)
{
	m_transform = owner->GetComponent<Transform>();
}

void Camera::Update()
{
}

const Matrix Camera::GetViewMatrix()
{
	XMVECTOR position = XMLoadFloat3(&m_transform->GetPosition());
	return XMMatrixLookAtLH(
		position,
		position + XMLoadFloat3(&m_transform->GetForward()),
		XMLoadFloat3(&m_transform->GetUp()));
}

const Matrix Camera::GetProjectionMatrix()
{
	if (!m_rendertarget)
		return Matrix();

	float aspect = (float)m_rendertarget->GetWidth() / (float)m_rendertarget->GetHeight();
	return XMMatrixPerspectiveFovLH(m_fov, aspect, m_near, m_far);
}