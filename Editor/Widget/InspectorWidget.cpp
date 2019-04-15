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
	if (!EditorHelper::sSelected || !m_visible)
		return;

	ImGui::Text("Name");
	char str[128];
	strcpy_s(str, 128, EditorHelper::sSelected->GetName().c_str());
	ImGui::InputText("##Name", str, 128);
	EditorHelper::sSelected->SetName(str);
	ImGui::NewLine();
	
	ShowTransform();
	ShowCamera();
	ShowMeshRender();
}

void InspectorWidget::ShowTransform()
{
	Transform* transform = EditorHelper::sSelected->GetComponent<Transform>();
	if (!transform)	return;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 position = transform->GetPosition();
		ImGui::Text("Position");
		ImGui::InputFloat3("##Position", &position.x);
		transform->SetPosition(position);

		Vector3 rotation = transform->GetEulerRotation();
		ImGui::Text("Rotation");
		ImGui::InputFloat3("##Rotation", &rotation.x);
		transform->SetEulerRotation(rotation);

		Vector3 scale = transform->GetScale();
		ImGui::Text("Scale");
		ImGui::InputFloat3("##Scale", &scale.x);
		transform->SetScale(scale);

		ImGui::NewLine();
	}
}

void InspectorWidget::ShowCamera()
{
	Camera* camera = EditorHelper::sSelected->GetComponent<Camera>();
	if (!camera)	return;

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float fov = camera->GetFOV();
		ImGui::Text("FOV");
		ImGui::SliderAngle("##Fov", &fov, 60.0f, 110.0f);
		camera->SetFOV(fov);

		float nearplane = camera->GetNearPlane();
		ImGui::Text("Near");
		ImGui::InputFloat("##Near", &nearplane, 1, 10, 1);
		camera->SetNearPlane(nearplane);

		float farplane = camera->GetFarPlane();
		ImGui::Text("Far");
		ImGui::InputFloat("##Far", &farplane, 1, 10, 1);
		camera->SetFarPlane(farplane);

		ImGui::NewLine();
	}
}

void InspectorWidget::ShowMeshRender()
{
	MeshRenderer* pMeshRenderer = EditorHelper::sSelected->GetComponent<MeshRenderer>();
	if (!pMeshRenderer)	return;

	if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char strbuf[128] = "";
		if (Mesh* pMesh = pMeshRenderer->GetMesh())
			strcpy_s(strbuf, 128, pMesh->GetName().c_str());
		ImGui::InputText("##Mesh", strbuf, 128);
		ImGui::NewLine();
		if (Material* pMaterial = pMeshRenderer->GetMaterial())
			strcpy_s(strbuf, 128, pMaterial->GetName().c_str());
		ImGui::InputText("##Material", strbuf, 128);
		ImGui::NewLine();
	}
}

/*
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
				for (size_t i = 0; i < value.size(); i++)
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
				strcpy_s(temp, value.get<std::string>().data());
				ImGui::InputText(newLabel.c_str(), temp, 128);
				if (hasKey) j[key] = temp;
				else j = temp;
			} break;
		}
		if (hasKey) 
			ImGui::Separator();
	}
}
*/