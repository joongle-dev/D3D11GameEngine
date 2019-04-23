#include "stdafx.h"
#include "InspectorWidget.h"
#include "nameof.hpp"

InspectorWidget::InspectorWidget(Context * context) :
	IWidget(context)
{
	mName = "Inspector";
	mWindowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
}

void InspectorWidget::Render()
{
	if (!EditorHelper::sSelected || !mIsVisible)
		return;

	ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());

	ImGui::Text("Name");
	char str[128];
	strcpy_s(str, 128, EditorHelper::sSelected->GetName().c_str());
	ImGui::InputText("##Name", str, 128);
	EditorHelper::sSelected->SetName(str);
	ImGui::NewLine();
	
	ShowTransform();
	ShowCamera();
	ShowMeshRender();
	ShowSkinnedMeshRender();
	ShowLight();

	ImGui::PopItemWidth();
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

	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
			ShowMesh(pMeshRenderer->GetMesh());
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
			ShowMaterial(pMeshRenderer->GetMaterial());
			ImGui::TreePop();
		}
	}
}

void InspectorWidget::ShowSkinnedMeshRender()
{
	SkinnedMeshRenderer* pMeshRenderer = EditorHelper::sSelected->GetComponent<SkinnedMeshRenderer>();
	if (!pMeshRenderer) return;

	if (ImGui::CollapsingHeader("Skinned Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Mesh");
		ShowMesh(pMeshRenderer->GetMesh());
		ImGui::Separator();
		ImGui::Text("Material");
		ShowMaterial(pMeshRenderer->GetMaterial());
		ImGui::Separator();
		ImGui::Text("Root Transform");
		std::string strbuf;
		GameObject* pObject = pMeshRenderer->GetRootTransform() ? pMeshRenderer->GetRootTransform()->GetOwner() : nullptr;
		if (pObject)
			strbuf = pObject->GetName();
		ImGui::Text("%s", strbuf.c_str());
	}
}

void InspectorWidget::ShowAnimator()
{
}

void InspectorWidget::ShowLight()
{
	Light* pLight = EditorHelper::sSelected->GetComponent<Light>();
	if (!pLight) return;

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{

		ImGui::Text("Lighting type:");
		const char* types[] = { "Directional", "Point", "Spot" };
		Light::LightType type = pLight->GetLightType();
		if (ImGui::BeginCombo("##LightType", types[type], ImGuiComboFlags_None))
		{
			bool bSelected = false;
			for (unsigned int i = 0; i < 3; i++)
				if (ImGui::Selectable(types[i], &bSelected))
					pLight->SetLightType(static_cast<Light::LightType>(i));
			if (bSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::EndCombo();
		}

		ImGui::Separator();
		ImGui::Text("Color:");
		Vector3 color = pLight->GetColor();
		if (ImGui::ColorEdit3("##LightColor", &color.x))
			pLight->SetColor(color);

		if (type != Light::Directional)
		{
			ImGui::Separator();
			ImGui::Text("Range:");
			float range = pLight->GetRange();
			if (ImGui::SliderFloat("##Range", &range, 0.1f, 1000.0f))
				pLight->SetRange(range);

			ImGui::Separator();
			ImGui::Text("Intensity:");
			float intensity = pLight->GetIntensity();
			if (ImGui::SliderFloat("##Intensity", &intensity, 0.0f, 1.0f))
				pLight->SetIntensity(intensity);
				
			if (type == Light::Spot)
			{
				ImGui::Separator();
				ImGui::Text("Spot Angle:");
				float angle = pLight->GetSpotAngle() * TO_DEG;
				if (ImGui::SliderFloat("##SpotAngle", &angle, 1.0f, 90.0f))
					pLight->SetSpotAngle(angle * TO_RAD);
			}
		}
	}
}

void InspectorWidget::ShowMaterial(Material * pMaterial)
{
	if (!pMaterial)
		return;

	char strbuf[128] = "";
	strcpy_s(strbuf, 128, pMaterial->GetName().c_str());
	ImGui::InputText("##MaterialName", strbuf, 128);
	pMaterial->SetName(strbuf);

	ResourceManager* pResourceManager = mContext->GetSubsystem<ResourceManager>();

	auto TextureSelectPopup = [pResourceManager](const char* label)->Texture*
	{
		Texture* pTextureSelected = nullptr;

		if (ImGui::BeginPopup(label))
		{
			for (Texture* texture : pResourceManager->GetResources<Texture>())
			{
				if (ImGui::ImageButton(texture->GetShaderResource(), ImVec2(100, 100)))
				{
					pTextureSelected = texture;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		
		return pTextureSelected;
	};

	for (unsigned int i = 0; i < Material::Unknown; i++)
	{
		Material::TextureType type = static_cast<Material::TextureType>(i);

		if (Texture* pTexture = pMaterial->GetTexture(type))
		{
			std::string label = "Textures##Popup" + std::to_string(i);

			if (ImGui::ImageButton(pTexture->GetShaderResource(), ImVec2(100, 100)))
				ImGui::OpenPopup(label.c_str());

			if (Texture* pTextureSelected = TextureSelectPopup(label.c_str()))
				pMaterial->SetTexture(type, pTextureSelected);

			if (Texture* pTexturePaylod = ImGuiDropTarget<Texture*>("TexturePayload"))
				pMaterial->SetTexture(type, pTexturePaylod);
		}
	}
}

void InspectorWidget::ShowMesh(Mesh * pMesh)
{
	if (!pMesh)
		return;

	char strbuf[128] = "";
	strcpy_s(strbuf, 128, pMesh->GetName().c_str());
	ImGui::InputText("##MeshName", strbuf, 128);
	pMesh->SetName(strbuf);

	ImGui::Text("Vertex count: %d", pMesh->GetVertexCount());
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