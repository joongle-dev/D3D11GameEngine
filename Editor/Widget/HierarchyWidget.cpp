#include "stdafx.h"
#include "HierarchyWidget.h"

HierarchyWidget::HierarchyWidget(Context * context) :
	IWidget(context)
{
	m_name = "Hierarchy";
}

void HierarchyWidget::Render()
{
	if (ImGui::IsWindowHovered(0) && ImGui::IsMouseClicked(0))
		EditorHelper::selected = nullptr;

	SceneNode(m_context->GetSubsystem<SceneManager>()->GetCurrentScene());
}

void HierarchyWidget::SceneNode(Scene * scene)
{
	Transform* rootnode = scene->GetRoot();
	
	//Check if this node is open
	bool open = ImGui::CollapsingHeader(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	//Node interaction
	DropTarget(rootnode);
	ContextMenu(scene, rootnode);
	
	//If node was open, display its child nodes (root transforms)
	if (open)
		for (size_t i = 0; i < rootnode->GetChildCount(); i++)
			ObjectNode(scene, rootnode->GetChild(i));
}

void HierarchyWidget::ObjectNode(Scene * scene, Transform * transform)
{
	GameObject* object = transform->GetOwner();
	std::string label = object->GetName() + "##" + std::to_string(object->GetInstanceID());
	
	//Node flags
	ImGuiTreeNodeFlags flags = (EditorHelper::selected == object ? 
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
		EditorHelper::selected = object;

	//If node was open, display its child nodes (this transform's children)
	if (open)
	{
		for (size_t i = 0; i < transform->GetChildCount(); i++)
			ObjectNode(scene, transform->GetChild(i));
		ImGui::TreePop();
	}
}

void HierarchyWidget::DragSource(Transform * src)
{
	if (ImGui::BeginDragDropSource(0))
	{
		//Set drag drop payload
		ImGui::SetDragDropPayload("HierarchyPayload", &src, sizeof(Transform*));
		ImGui::EndDragDropSource();
	}
}

void HierarchyWidget::DropTarget(Transform * dst)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HierarchyPayload", 0))
		{
			//Drag source transform
			Transform* src = *static_cast<Transform**>(payload->Data);
			//This lambda function checks if the first parameter is an ancestor of second parameter
			auto AncestorCheck = [](Transform* ancestor, Transform* descendant, const auto& lambda)->bool
			{
				if (descendant->GetParent() == ancestor)
					return true;
				if (descendant->GetParent() == nullptr)
					return false;
				return lambda(ancestor, descendant->GetParent(), lambda);
			};
			//If drag source is ancestor of drop target, set source's parent as target's parent
			if (AncestorCheck(src, dst, AncestorCheck))
				dst->SetParent(src->GetParent());
			//Set drop target as drag source's parent
			src->SetParent(dst);
		}
		ImGui::EndDragDropTarget();
	}
}

void HierarchyWidget::ContextMenu(Scene * scene, Transform * transform)
{
	if (ImGui::BeginPopupContextItem("HierarchyMenu"))
	{
		if (ImGui::MenuItem("Delete"))       DestroyObject(scene, transform);
		ImGui::Separator();
		if (ImGui::MenuItem("Create Empty")) CreateObject(scene, transform);
		if (ImGui::BeginMenu("Create 3D"))
		{
			if (ImGui::MenuItem("Cube"))     CreateObject(scene, transform, "Cube");
			if (ImGui::MenuItem("Sphere"))   CreateObject(scene, transform, "Sphere");
			ImGui::EndMenu();
		}
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

void HierarchyWidget::DestroyObject(Scene * scene, Transform * transform)
{
	scene->Destroy(transform->GetOwner());
	EditorHelper::selected = nullptr;
}
