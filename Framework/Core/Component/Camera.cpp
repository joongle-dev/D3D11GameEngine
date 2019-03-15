#include "Framework.h"
#include "Camera.h"


using namespace DirectX;

Camera::Camera(Context * context, GameObject* owner) :
	Component<Camera>(context, owner),
	m_near(0.1f), m_far(1000.0f), m_fov(XM_PIDIV2), m_type(ProjectionType::Perspective)
{
	m_renderer = context->GetSubsystem<Renderer>();
	m_transform = owner->GetComponent<Transform>();

	m_renderer->SetCamera(this);
}

void Camera::Update()
{
	XMFLOAT3 position = m_transform->GetPosition();
	XMFLOAT3 forward = m_transform->GetForward();
	XMFLOAT3 right = m_transform->GetRight();
	XMFLOAT3 up = m_transform->GetUp();

	Input* input = m_context->GetSubsystem<Input>();
	float elapsed = m_context->GetSubsystem<Time>()->Elapsed();
	float speed = elapsed * 15.0f;

	if (input->KeyDown(KeyCode::W)) {
		position.x += forward.x * speed;
		position.y += forward.y * speed;
		position.z += forward.z * speed;
	}
	if (input->KeyDown(KeyCode::S)) {
		position.x -= forward.x * speed;
		position.y -= forward.y * speed;
		position.z -= forward.z * speed;
	}
	if (input->KeyDown(KeyCode::A)) {
		position.x -= right.x * speed;
		position.y -= right.y * speed;
		position.z -= right.z * speed;
	}
	if (input->KeyDown(KeyCode::D)) {
		position.x += right.x * speed;
		position.y += right.y * speed;
		position.z += right.z * speed;
	}
	if (input->KeyDown(KeyCode::Q)) {
		position.x += up.x * speed;
		position.y += up.y * speed;
		position.z += up.z * speed;
	}
	if (input->KeyDown(KeyCode::E)) {
		position.x -= up.x * speed;
		position.y -= up.y * speed;
		position.z -= up.z * speed;
	}

	m_transform->SetPosition(position);

	if (input->KeyDown(VK_MBUTTON)) {
		XMFLOAT2 mouse = input->GetMouseMove();
		static XMFLOAT3 angles = { 0, 0, 0 };
		angles.x += mouse.y * elapsed;
		angles.y += mouse.x * elapsed;
		m_transform->SetRotationEuler(angles);
	}
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