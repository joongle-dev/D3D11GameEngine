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

const XMFLOAT4X4 Camera::GetViewMatrix()
{
	XMFLOAT4X4 view;
	XMVECTOR position = XMLoadFloat3(&m_transform->GetPosition());
	XMStoreFloat4x4(&view, XMMatrixLookAtLH(
		position,
		position + XMLoadFloat3(&m_transform->GetForward()),
		XMLoadFloat3(&m_transform->GetUp())));
	return view;
}

const XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	XMFLOAT4X4 projection;
	float aspect = m_context->GetSubsystem<Graphics>()->GetAspectRatio();
	XMStoreFloat4x4(&projection, 
		XMMatrixPerspectiveFovLH(m_fov, aspect, m_near, m_far));
	return projection;
}