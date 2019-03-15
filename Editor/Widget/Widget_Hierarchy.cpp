#include "stdafx.h"
#include "Widget_Hierarchy.h"

Widget_Hierarchy::Widget_Hierarchy(Context * context) :
	IWidget(context)
{
}

void Widget_Hierarchy::Render()
{
	AddScene(m_context->GetSubsystem<SceneManager>()->GetCurrentScene());
}

void Widget_Hierarchy::AddScene(Scene * scene)
{
	if (!scene) return;

	if (ImGui::CollapsingHeader(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		Transform* root = scene->GetRoot();
		for (int i = 0; i < root->GetChildCount(); i++)
			AddObject(root->GetChild(i));
	}
}

void Widget_Hierarchy::AddObject(Transform * parent)
{
	if (!parent) return;

	if (ImGui::TreeNodeEx(parent->GetOwner()->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < parent->GetChildCount(); i++)
			AddObject(parent->GetChild(i));
		ImGui::TreePop();
	}
}
