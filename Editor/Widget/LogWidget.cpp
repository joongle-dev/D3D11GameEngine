#include "stdafx.h"
#include "LogWidget.h"

LogWidget::LogWidget(Context * context) :
	IWidget(context),
	mShowInfo(true),
	mShowWarning(true),
	mShowError(true)
{
	mName = "Log";

	Log::SetLogFunction([this](LogType type, const char* text) { AddLog(type, text); });

	mIcons.emplace_back(new Texture(context))->LoadFromFile("../Assets/Icon/information_blue_32.png");
	mIcons.emplace_back(new Texture(context))->LoadFromFile("../Assets/Icon/warning_yellow_32.png");
	mIcons.emplace_back(new Texture(context))->LoadFromFile("../Assets/Icon/error_red_32.png");
}

void LogWidget::Render()
{
	auto ShowButton = [this](LogType type, bool& toggle)
	{
		ImGui::PushStyleColor(
			ImGuiCol_Button, 
			ImGui::GetStyle().Colors[toggle ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
		if (ImGui::ImageButton(mIcons[type]->GetShaderResource(), ImVec2(16.0f, 16.0f)))
			toggle = !toggle;
		ImGui::PopStyleColor();
		ImGui::SameLine();
	};

	ShowButton(LogType_Info, mShowInfo);
	ShowButton(LogType_Warning, mShowWarning);
	ShowButton(LogType_Error, mShowError);

	ImGui::Separator();

	static const char* types[] = { "Info", "Warning", "Error" };
	static const ImVec4 colors[] = { ImVec4(0.75f, 0.75f, 0.75f, 1.0f), ImVec4(0.75f, 0.75f, 0.0f, 1.0f), ImVec4(0.75f, 0.0f, 0.0f, 1.0f) };

	ImGui::BeginChild("Logs", ImVec2(0, 0), true);
	{
		for (auto log : mLogs)
		{
			if ((log.type == LogType_Info && mShowInfo) ||
				(log.type == LogType_Warning && mShowWarning) ||
				(log.type == LogType_Error && mShowError))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, colors[log.type]);
				ImGui::Text("[%s]: %s", types[log.type], log.text.c_str());
				ImGui::PopStyleColor();
			}
			
		}
	}
	ImGui::EndChild();
}

void LogWidget::AddLog(LogType type, const char * text)
{
	mLogs.emplace_back(type, text);
}
