#include "stdafx.h"
#include "LogWidget.h"

LogWidget::LogWidget(Context * context) :
	IWidget(context),
	mShowInfo(true),
	mShowWarning(true),
	mShowError(true)
{
	mName = "Log";
}

void LogWidget::Render()
{
	auto ShowButton = [](const char* label)
	{
		if (ImGui::Button(label, ImVec2(100, 25)));
		ImGui::SameLine();
	};

	ShowButton("ShowInfo");
	ShowButton("ShowWarning");
	ShowButton("ShowError");
	ImGui::Separator();
	ImGui::BeginChild("Logs", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
	{

	}
	ImGui::EndChild();
}
