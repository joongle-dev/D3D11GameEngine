#include "stdafx.h"
#include "AssetWidget.h"

AssetWidget::AssetWidget(Context * context) :
	IWidget(context)
{
	mName = "Asset";

	mResourceManager = mContext->GetSubsystem<ResourceManager>();
}

void AssetWidget::Render()
{
	if (ImGui::BeginTabBar("AssetTabs"))
	{
		if (ImGui::BeginTabItem("Textures##AssetTabs"))
		{
			for (Texture* texture : mResourceManager->GetResources<Texture>())
			{
				if (ImGui::ImageButton(texture->GetShaderResource(), ImVec2(100, 100)));
				ImGuiDragSource<Texture*>("TexturePayload", texture);

				ImGui::SameLine();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Meshes##AssetTabs"))
		{
			for (Mesh* mesh : mResourceManager->GetResources<Mesh>())
			{
				ImGui::Text("%s", mesh->GetName());

				//ImGui::SameLine();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}