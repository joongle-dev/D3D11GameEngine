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
	ImGui::NewLine();

	//Edit components
	for (size_t index = 0; index < Util::FamilyTypeID<IComponent>::GetCount(); index++)
		if (auto component = (*EditorHelper::selected)[index])
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::CollapsingHeader(component->GetComponentName().c_str(), flags)) {
				auto componentdata = component->Serialize();
				JsonEditor("##", componentdata);
				component->Deserialize(componentdata);
				ImGui::NewLine();
			}
		}

	//ImGui::Text("Name");
	//std::string str = EditorHelper::selected->GetName();
	//str.reserve(128);
	//ImGui::InputText("##Name", str.data(), 128);
	//EditorHelper::selected->SetName(str);
	//ImGui::NewLine();
	//
	//ShowTransform();
	//ShowCamera();
}

void InspectorWidget::ShowTransform()
{
	Transform* transform = EditorHelper::selected->GetComponent<Transform>();
	if (!transform)	return;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 position = transform->GetPosition();
		ImGui::Text("Position");
		ImGui::InputFloat("##PositionX", &position.x, 1.0f, 1.0f, 3, 0);
		ImGui::InputFloat("##PositionY", &position.y, 1.0f, 1.0f, 3, 0);
		ImGui::InputFloat("##PositionZ", &position.z, 1.0f, 1.0f, 3, 0);
		transform->SetPosition(position);
		Vector3 rotation = transform->GetEulerRotation();
		ImGui::Text("Rotation");
		ImGui::InputFloat("##RotationX", &rotation.x, 1.0f, 1.0f, 3, 0);
		ImGui::InputFloat("##RotationY", &rotation.y, 1.0f, 1.0f, 3, 0);
		ImGui::InputFloat("##RotationZ", &rotation.z, 1.0f, 1.0f, 3, 0);
		transform->SetEulerRotation(rotation);
		Vector3 scale = transform->GetScale();
		ImGui::Text("Scale");
		ImGui::InputFloat("##ScaleX", &scale.x, 1.0f, 1.0f, 3, 0);
		ImGui::InputFloat("##ScaleY", &scale.y, 1.0f, 1.0f, 3, 0);
		ImGui::InputFloat("##ScaleZ", &scale.z, 1.0f, 1.0f, 3, 0);
		transform->SetScale(scale);
		ImGui::NewLine();
	}
}

void InspectorWidget::ShowCamera()
{
	Camera* camera = EditorHelper::selected->GetComponent<Camera>();
	if (!camera)	return;

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float fov = camera->GetFOV();
		ImGui::Text("FOV");
		ImGui::InputFloat("##Fov", &fov, 1.0f, 1.0f, 1, 0);
		camera->SetFOV(fov);
		float nearplane = camera->GetNearPlane();
		ImGui::Text("Near");
		ImGui::InputFloat("##Near", &nearplane, 1.0f, 1.0f, 1, 0);
		camera->SetNearPlane(nearplane);
		float farplane = camera->GetFarPlane();
		ImGui::Text("Far");
		ImGui::InputFloat("##Far", &farplane, 1.0f, 1.0f, 1, 0);
		camera->SetFarPlane(farplane);
		ImGui::NewLine();
	}
}

void InspectorWidget::JsonEditor(std::string label, Json & j)
{
	for (auto& [key, value] : j.items())
	{
		bool hasKey = key.length();
		if (hasKey)
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
				ImGui::InputFloat(newLabel.c_str(), &temp, 1.0f, 1.0f, 3);
				if (hasKey) j[key] = temp;
				else j = temp;
			} break;
			case Json::value_t::number_integer: {
				int temp = value.get<int>();
				ImGui::InputInt(newLabel.c_str(), &temp, 1, 10);
				if (hasKey) j[key] = temp;
				else j = temp;
			} break;
			case Json::value_t::string: {
				char temp[128];
				strcpy(temp, value.get<std::string>().data());
				ImGui::InputText(newLabel.c_str(), temp, 128);
				if (hasKey) j[key] = temp;
				else j = temp;
			} break;
		}
		if (hasKey) 
			ImGui::Separator();
	}
}
