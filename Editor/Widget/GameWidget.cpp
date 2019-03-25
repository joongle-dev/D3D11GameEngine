#include "stdafx.h"
#include "GameWidget.h"

GameWidget::GameWidget(Context * context) :
	IWidget(context)
{
	m_name = "Game";
	m_rendertarget = context->GetSubsystem<Renderer>()->GetMainTarget();
}

void GameWidget::Render()
{
	if (!m_visible)
		return;

	//Adjust image size, cursor position and draw scene
	float scale = std::max(ImGui::GetWindowSize().x / m_rendertarget->GetWidth(), ImGui::GetWindowSize().y / m_rendertarget->GetHeight());
	ImVec2 frameSize = ImVec2(m_rendertarget->GetWidth() * scale, m_rendertarget->GetHeight() * scale);
	ImVec2 framePos = ImVec2((ImGui::GetWindowSize().x - frameSize.x) * 0.5f, (ImGui::GetWindowSize().y - frameSize.y) * 0.5f);
	ImGui::SetCursorPos(framePos);
	ImGui::Image(m_rendertarget->GetShaderResource(), frameSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImColor(255, 127, 166, 155));
}
