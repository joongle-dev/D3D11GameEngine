#include "stdafx.h"
#include "SceneWidget.h"

SceneWidget::SceneWidget(Context * context) :
	IWidget(context)
{
	m_name = "Scene";
	m_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	m_rendertarget = std::make_unique<RenderTarget>(context);
	m_rendertarget->Create(1280, 720);

	m_camera = std::make_unique<GameObject>(context, context->GetSubsystem<SceneManager>()->GetCurrentScene());
	m_camera->AddComponent<Camera>()->SetRenderTarget(m_rendertarget.get());
}

void SceneWidget::Render()
{
	CameraControl();
	ImVec2 origin = ImGui::GetCursorScreenPos();

	//Adjust image size, cursor position and draw scene
	float scale = max(ImGui::GetWindowSize().x / m_rendertarget->GetWidth(), ImGui::GetWindowSize().y / m_rendertarget->GetHeight());
	ImVec2 imageSize = ImVec2(m_rendertarget->GetWidth() * scale, m_rendertarget->GetHeight() * scale);
	ImVec2 cursorPos = ImVec2((ImGui::GetWindowSize().x - imageSize.x) * 0.5f, (ImGui::GetWindowSize().y - imageSize.y) * 0.5f);
	ImGui::SetCursorPos(cursorPos);
	ImGui::Image(m_rendertarget->GetShaderResource(), imageSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImColor(255, 127, 166, 155));

	//Scene overlay
	ImGui::SetNextWindowPos(origin);
	ImGui::BeginChild("Overlay", ImVec2(200, 200));
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "FPS: %.0f", ImGui::GetIO().Framerate);
	ImGui::EndChild();
}

void SceneWidget::CameraControl()
{
	if (ImGui::IsWindowFocused()) 
	{
		float elapsed = m_context->GetSubsystem<Time>()->Elapsed();
		Input* input = m_context->GetSubsystem<Input>();
		Transform* transform = m_camera->GetComponent<Transform>();

		//Keyboard
		float speed = 1.0f * elapsed;
		Vector3 forward = transform->GetForward();
		Vector3 right = transform->GetRight();
		Vector3 up = transform->GetUp();
		Vector3 move;
		if (input->KeyDown(KeyCode::W)) move += forward * speed;
		if (input->KeyDown(KeyCode::S)) move -= forward * speed;
		if (input->KeyDown(KeyCode::D)) move += right * speed;
		if (input->KeyDown(KeyCode::A)) move -= right * speed;
		if (input->KeyDown(KeyCode::Q)) move += up * speed;
		if (input->KeyDown(KeyCode::E)) move -= up * speed;
		move.Normalize();
		transform->Translate(move);

		//Mouse
		ImGui::GetIO().KeyRepeatDelay = 0.00f;
		if (ImGui::IsMouseClicked(2, true)) {
			float sensitivity = 1.0f * elapsed;
			Vector2 mouse = input->GetMouseMove();
			m_angles.y += mouse.x * sensitivity;
			m_angles.x += mouse.y * sensitivity;
			transform->SetRotationEuler(Vector3(m_angles.x, m_angles.y, 0));
		}
		ImGui::GetIO().KeyRepeatDelay = 0.25f;
	}
}
