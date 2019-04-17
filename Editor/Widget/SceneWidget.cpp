#include "stdafx.h"
#include "SceneWidget.h"
#include "../ImGui/ImGuizmo.h"

SceneWidget::SceneWidget(Context * context) :
	IWidget(context)
{
	mName = "Scene";
	mWindowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	m_rendertarget = std::make_unique<RenderTarget>(context);
	m_rendertarget->Create(1280, 720);

	m_camera = std::make_unique<GameObject>(context, context->GetSubsystem<SceneManager>()->GetCurrentScene());
	m_camera->AddComponent<Camera>()->SetRenderTarget(m_rendertarget.get());
}

void SceneWidget::Render()
{
	if (!mIsVisible)
		return;

	CameraControl();
	ShowFrame();
	ShowGizmo();
}

void SceneWidget::CameraControl()
{
	if (ImGui::IsWindowFocused()) 
	{
		float elapsed = mContext->GetSubsystem<Time>()->Elapsed();
		Input* input = mContext->GetSubsystem<Input>();
		Transform* transform = m_camera->GetComponent<Transform>();

		//Keyboard
		float speed = 2.0f * elapsed;
		Vector3 forward = transform->GetForward();
		Vector3 right = transform->GetRight();
		Vector3 up = transform->GetUp();
		Vector3 move;
		if (input->KeyDown(KeyCode::W)) move += forward;
		if (input->KeyDown(KeyCode::S)) move -= forward;
		if (input->KeyDown(KeyCode::D)) move += right;
		if (input->KeyDown(KeyCode::A)) move -= right;
		if (input->KeyDown(KeyCode::Q)) move += up;
		if (input->KeyDown(KeyCode::E)) move -= up;
		move.Normalize();
		transform->Translate(move * speed);

		//Mouse
		static ImVec2 mouse = ImVec2(0, 0);
		if (ImGui::IsMouseDragging(2)) {
			mouse = ImGui::GetMouseDragDelta(2);
			transform->SetEulerRotation(Vector3(m_angles.x + mouse.y, m_angles.y + mouse.x, 0));
		}
		else {
			m_angles.x += mouse.y;
			m_angles.y += mouse.x;
			mouse = ImVec2(0, 0);
		}
	}
}

void SceneWidget::ShowFrame()
{
	ImVec2 origin = ImGui::GetCursorScreenPos();

	//Adjust image size, cursor position and draw scene
	float scale = std::max(ImGui::GetWindowSize().x / m_rendertarget->GetWidth(), ImGui::GetWindowSize().y / m_rendertarget->GetHeight());
	frameSize = ImVec2(m_rendertarget->GetWidth() * scale, m_rendertarget->GetHeight() * scale);
	framePos = ImVec2((ImGui::GetWindowSize().x - frameSize.x) * 0.5f, (ImGui::GetWindowSize().y - frameSize.y) * 0.5f);
	ImGui::SetCursorPos(framePos);
	ImGui::Image(m_rendertarget->GetShaderResource(), frameSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImColor(255, 127, 166, 155));

	//Scene overlay
	ImGui::SetNextWindowPos(origin);
	ImGui::BeginChild("Overlay", ImVec2(200, 200));
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "FPS: %.0f", ImGui::GetIO().Framerate);
	ImGui::EndChild();
}

void SceneWidget::ShowGizmo()
{
	if (!EditorHelper::sSelected)
		return;

	auto camera = m_camera->GetComponent<Camera>();
	auto transform = EditorHelper::sSelected->GetComponent<Transform>();

	static ImGuizmo::OPERATION operation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(87))
		operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		operation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82))
		operation = ImGuizmo::SCALE;

	Matrix world = transform->GetWorldTransform();
	Matrix view = camera->GetViewMatrix();
	Matrix proj = camera->GetProjectionMatrix();
	
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(framePos.x + ImGui::GetWindowPos().x, framePos.y + ImGui::GetWindowPos().y, frameSize.x, frameSize.y);
	ImGuizmo::Manipulate(*view.m, *proj.m, operation, mode, *world.m);

	DirectX::XMVECTOR s, r, t;
	DirectX::XMMatrixDecompose(&s, &r, &t, world);

	transform->SetPosition(t);
	transform->SetRotation(r);
	transform->SetScale(s);
}
