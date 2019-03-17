#include "stdafx.h"
#include "HierarchyWidget.h"

HierarchyWidget::HierarchyWidget(Context * context) :
	IWidget(context)
{
	m_name = "Hierarchy";
}

void HierarchyWidget::Render()
{
	SceneNode(m_context->GetSubsystem<SceneManager>()->GetCurrentScene());
}

void HierarchyWidget::SceneNode(Scene * scene)
{
	Transform* root = scene->GetRoot();

	//Node flags
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

	//Check if this node is open
	bool open = ImGui::CollapsingHeader(scene->GetName().c_str(), flags);
	
	//Node interaction
	DropTarget(root);
	ContextMenu(scene, root);
	
	//If node was open, display its child nodes (root transforms)
	if (open)
	{
		for (int i = 0; i < root->GetChildCount(); i++)
			ObjectNode(scene, root->GetChild(i));
	}
}

void HierarchyWidget::ObjectNode(Scene * scene, Transform * transform)
{
	GameObject* object = transform->GetOwner();
	std::string label = object->GetName() + "##" + std::to_string(object->GetInstanceID());
	
	//Node flags
	ImGuiTreeNodeFlags flags = (m_selected == object ? 
		ImGuiTreeNodeFlags_Selected : 0) |
		ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_OpenOnArrow;

	//Check if this node is open
	bool open = ImGui::TreeNodeEx(label.c_str(), flags);

	//Node interaction
	DragSource(transform);
	DropTarget(transform);
	ContextMenu(scene, transform);
	if (ImGui::IsItemClicked(0))
		m_selected = object;

	//If node was open, display its child nodes (this transform's children)
	if (open)
	{
		for (int i = 0; i < transform->GetChildCount(); i++)
			ObjectNode(scene, transform->GetChild(i));
		ImGui::TreePop();
	}
}

void HierarchyWidget::DragSource(Transform * src)
{
	ImGuiDragDropFlags flags = ImGuiDragDropFlags_SourceNoDisableHover;

	if (ImGui::BeginDragDropSource(flags))
	{
		//Set drag drop payload
		ImGui::SetDragDropPayload("HierarchyDrag", &src, sizeof(Transform*));

		ImGui::EndDragDropSource();
	}
}

void HierarchyWidget::DropTarget(Transform * dst)
{
	ImGuiDragDropFlags flags = 0;

	if (ImGui::BeginDragDropTarget())
	{
		//Receive drag drop payload
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyDrag", flags))
			//Add received transfrom to children
			dst->AddChild(*static_cast<Transform**>(payload->Data));

		ImGui::EndDragDropTarget();
	}
}

void HierarchyWidget::ContextMenu(Scene * scene, Transform * parent)
{
	if (ImGui::BeginPopupContextItem("Create Object"))
	{
		if (ImGui::MenuItem("Empty Object")) CreateObject(scene, parent);
		if (ImGui::MenuItem("Cube"))         CreateObject(scene, parent, "Cube");
		if (ImGui::MenuItem("Sphere"))       CreateObject(scene, parent, "Sphere");

		ImGui::EndPopup();
	}
}

void HierarchyWidget::CreateObject(Scene * scene, Transform * parent, const std::string & name)
{
	//Instantiate GameObject, and set its parent and name
	auto object = scene->Instantiate();
	object->GetComponent<Transform>()->SetParent(parent);
	object->SetName(name);

	Geometry geometry;
	//Create Cube
	if (name == "Cube")
	{
		Geometry::CreateCube(geometry, 10.0f);
		Mesh* mesh = new Mesh(m_context);
		mesh->Create(geometry);
		object->AddComponent<MeshRenderer>()->SetMesh(mesh);
	}
	//Create Sphere
	else if (name == "Sphere")
	{
		Geometry::CreateSphere(geometry, 5.0f);
		Mesh* mesh = new Mesh(m_context);
		mesh->Create(geometry);
		object->AddComponent<MeshRenderer>()->SetMesh(mesh);
	}
}
