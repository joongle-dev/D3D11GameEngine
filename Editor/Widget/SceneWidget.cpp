#include "stdafx.h"
#include "SceneWidget.h"

SceneWidget::SceneWidget(Context * context) :
	IWidget(context)
{
	m_name = "Scene";
	m_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	m_rendertarget = std::make_unique<RenderTarget>(context);
	m_rendertarget->Create(1280, 720);

	m_camera = std::make_unique<GameObject>(context);
	m_camera->AddComponent<Camera>()->SetRenderTarget(m_rendertarget.get());
}

void SceneWidget::Render()
{
	CameraControl();

	ImVec2 frameSize;
	frameSize.x = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
	frameSize.y = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

	frameSize.x -= static_cast<int>(frameSize.x) % 2 != 0 ? 1 : 0;
	frameSize.y -= static_cast<int>(frameSize.y) % 2 != 0 ? 1 : 0;

	ImGui::Image(m_rendertarget->GetShaderResource(), frameSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImColor(50, 127, 166, 155));
}

void SceneWidget::CameraControl()
{
	Input* input = m_context->GetSubsystem<Input>();
	Transform* transform = m_camera->GetComponent<Transform>();
	Time* time = m_context->GetSubsystem<Time>();

	if (ImGui::IsWindowFocused()) {
		Vector3 forward = transform->GetForward();
		Vector3 right = transform->GetRight();
		float speed = 1.0f * time->Elapsed();
		Vector3 move;
		if (input->KeyDown(KeyCode::W))
			move += forward * speed;
		if (input->KeyDown(KeyCode::S))
			move -= forward * speed;
		if (input->KeyDown(KeyCode::D))
			move += right * speed;
		if (input->KeyDown(KeyCode::A))
			move -= right * speed;
		move.Normalize();
		transform->Translate(move);
	}

	ImGui::GetCursorPos();
}
