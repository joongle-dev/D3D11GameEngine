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
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Transform"))
			{
				root->AddChild(scene->GetByID(*static_cast<uint32_t*>(payload->Data))->GetComponent<Transform>());
			}
			ImGui::EndDragDropTarget();
		}
		ContextMenu(scene, root);
		for (int i = 0; i < root->GetChildCount(); i++)
			AddObject(scene, root->GetChild(i));
	}
}

void Widget_Hierarchy::AddObject(Scene * scene, Transform * transform)
{
	GameObject* object = transform->GetOwner();
	auto label = object->GetName() + "##" + std::to_string(object->GetInstanceID());

	bool open = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover))
	{
		ImGui::SetDragDropPayload("Transform", &object->GetInstanceID(), sizeof(uint32_t));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Transform"))
		{
			transform->AddChild(scene->GetByID(*static_cast<uint32_t*>(payload->Data))->GetComponent<Transform>());
		}
		ImGui::EndDragDropTarget();
	}
	ContextMenu(scene, transform);
	if (open)
	{
		for (int i = 0; i < transform->GetChildCount(); i++)
			AddObject(scene, transform->GetChild(i));
		ImGui::TreePop();
	}
}

void Widget_Hierarchy::ContextMenu(Scene * scene, Transform * parent)
{
	if (ImGui::BeginPopupContextItem("Create Object"))
	{
		if (ImGui::MenuItem("Empty Object")) CreateObject(scene, parent);
		if (ImGui::MenuItem("Cube"))         CreateObject(scene, parent, "Cube");
		if (ImGui::MenuItem("Sphere"))       CreateObject(scene, parent, "Sphere");
		ImGui::EndPopup();
	}
}

void Widget_Hierarchy::CreateObject(Scene * scene, Transform * parent, std::string name)
{
	auto object = scene->Instantiate();
	object->GetComponent<Transform>()->SetParent(parent);
	object->SetName(name);
	Transform* trans = object->GetComponent<Transform>();

	Geometry geometry;
	if (name == "Cube")
	{
		Geometry::CreateCube(geometry);
		Mesh* mesh = new Mesh(m_context);
		mesh->Create(geometry);
		object->AddComponent<MeshRenderer>()->SetMesh(mesh);
	}
	else if (name == "Sphere")
	{
		Geometry::CreateSphere(geometry);
		Mesh* mesh = new Mesh(m_context);
		mesh->Create(geometry);
		object->AddComponent<MeshRenderer>()->SetMesh(mesh);
	}
}
