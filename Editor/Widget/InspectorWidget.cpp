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

	auto objectdata = EditorHelper::selected->Serialize();
	JsonEditor("##", objectdata);
	//EditorHelper::selected->Deserialize(objectdata);

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
	for (auto& element : j.items())
	{
		element.value().type();
		if (element.key().length())
			ImGui::Text(element.key().c_str());
		std::string newLabel = label + element.key();
		switch (element.value().type()) {
			case Json::value_t::object: {
				JsonEditor(newLabel, element.value());
			} break;
			case Json::value_t::array: {
				for (int i = 0; i < element.value().size(); i++)
					JsonEditor(newLabel + std::to_string(i), element.value().at(i));
			} break;
			case Json::value_t::number_float: {
				float temp = element.value().get<float>();
				ImGui::InputFloat(newLabel.c_str(), &temp);
				j = temp;
			} break;
			case Json::value_t::number_integer: {
				int temp = element.value().get<int>();
				ImGui::InputInt(newLabel.c_str(), &temp);
				j = temp;
			} break;
			case Json::value_t::string: {
				std::string temp = element.value().get<std::string>().data();
				temp.reserve(256);
				ImGui::InputText(newLabel.c_str(), temp.data(), 256);
				j;
			} break;
		}
		if (element.key().length())
			ImGui::Separator();
	}
}
