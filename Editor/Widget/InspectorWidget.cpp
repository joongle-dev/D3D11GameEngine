#include "stdafx.h"
#include "InspectorWidget.h"

InspectorWidget::InspectorWidget(Context * context) :
	IWidget(context)
{
	m_name = "Inspector";
}

void InspectorWidget::Render()
{
	if (!EditorHelper::selected || !m_visible)
		return;

	for (size_t index = 0; index < Util::FamilyTypeID<IComponent>::GetCount(); index++)
	{
		if (auto component = (*EditorHelper::selected)[index])
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::CollapsingHeader(component->GetComponentName().c_str(), flags)) {
				auto data = component->Serialize();
				ParseData(data);
				component->Deserialize(data);
			}
		}
	}
}

void InspectorWidget::ParseData(nlohmann::json & j)
{
	for (auto& element : j.items())
	{
		ImGui::Text(element.key().c_str());

		switch (element.value().type())
		{
			case nlohmann::json::value_t::object:
			{
				ParseData(element.value());
				break;
			}
			case nlohmann::json::value_t::array:
			{
				size_t size = element.value().size();
				float temp[4];
				for (int i = 0; i < size; i++) {
					std::string label = "##" + element.key() + std::to_string(i);
					temp[i] = element.value().at(i).get<float>();
					ImGui::InputFloat(label.c_str(), temp + i);
					element.value().at(i) = temp[i];
				}
				break;
			}
			//TODO: string, integer, pointer... types
		}
		ImGui::Separator();
	}
}
