#include "stdafx.h"
#include "InspectorWidget.h"
#include "json.hpp"

InspectorWidget::InspectorWidget(Context * context) :
	IWidget(context)
{
	m_name = "Inspector";
}

void InspectorWidget::Render()
{
	if (!EditorHelper::selected || !m_visible)
		return;

	//Edit GameObject
	auto objectdata = EditorHelper::selected->Serialize();
	JsonEditor("##", objectdata);
	EditorHelper::selected->Deserialize(objectdata);

	//Edit components
	for (size_t index = 0; index < Util::FamilyTypeID<IComponent>::GetCount(); index++)
		if (auto component = (*EditorHelper::selected)[index])
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::CollapsingHeader(component->GetComponentName().c_str(), flags)) {
				auto componentdata = component->Serialize();
				JsonEditor("##", componentdata);
				component->Deserialize(componentdata);
			}
		}
}

void InspectorWidget::JsonEditor(std::string label, Json & j)
{
	for (auto& [key, value] : j.items())
	{
		if (key.length())
			ImGui::Text(key.c_str());
		std::string newLabel = label + key;
		switch (value.type()) {
			case Json::value_t::object: {
				JsonEditor(newLabel, value);
			} break;
			case Json::value_t::array: {
				for (int i = 0; i < value.size(); i++)
					JsonEditor(newLabel + std::to_string(i), value.at(i));
			} break;
			case Json::value_t::number_float: {
				float temp = value.get<float>();
				ImGui::InputFloat(newLabel.c_str(), &temp);
				j = temp;
			} break;
			case Json::value_t::number_integer: {
				int temp = value.get<int>();
				ImGui::InputInt(newLabel.c_str(), &temp);
				j = temp;
			} break;
			case Json::value_t::string: {
				char temp[128];
				strcpy(temp, value.get<std::string>().data());
				ImGui::InputText(newLabel.c_str(), temp, 256);
				j[key] = temp;
			} break;
		}
		if (key.length())
			ImGui::Separator();
	}
}
