#include "stdafx.h"
#include "HierarchyWidget.h"

HierarchyWidget::HierarchyWidget(Context * context) :
	IWidget(context)
{
	mName = "Hierarchy";
}

void HierarchyWidget::Render()
{
	if (ImGui::IsWindowHovered(0) && ImGui::IsMouseClicked(0))
		EditorHelper::sSelected = nullptr;

	SceneNode(mContext->GetSubsystem<SceneManager>()->GetCurrentScene());
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
	ImGuiTreeNodeFlags flags = (EditorHelper::sSelected == object ? 
		ImGuiTreeNodeFlags_Selected : 0) |
		ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_OpenOnArrow;

	//Check if this node is open
	bool open = ImGui::TreeNodeEx(label.c_str(), flags);

	//Node interaction
	ImGui::PushID(label.c_str());
	DragSource(transform);
	DropTarget(transform);
	ContextMenu(scene, transform);
	if (ImGui::IsItemClicked(0))
		EditorHelper::sSelected = object;
	ImGui::PopID();

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
	ImGuiDragSource<Transform*>("HierarchyPayload", src);
}

void HierarchyWidget::DropTarget(Transform * dst)
{
	//This lambda function checks if the first parameter is an ancestor of second parameter
	auto AncestorCheck = [](Transform* ancestor, Transform* descendant, const auto& lambda)->bool
	{
		if (descendant->GetParent() == ancestor)
			return true;
		if (descendant->GetParent() == nullptr)
			return false;
		return lambda(ancestor, descendant->GetParent(), lambda);
	};

	//Receive drag drop payload
	if (Transform* src = ImGuiDropTarget<Transform*>("HierarchyPayload"))
	{
		//If drag source is ancestor of drop target, set source's parent as target's parent
		//This swaps the source and destination transforms in hierarchy
		if (AncestorCheck(src, dst, AncestorCheck))
			dst->SetParent(src->GetParent());

		//Set drop target as drag source's parent
		src->SetParent(dst);
	}
}

void HierarchyWidget::ContextMenu(Scene * scene, Transform * transform)
{
	if (ImGui::BeginPopupContextItem("HierarchyMenu"))
	{
		if (transform->GetParent()) 
		{
			if (ImGui::MenuItem("Delete"))
			{
				scene->Destroy(transform->GetOwner());
				EditorHelper::sSelected = nullptr;

			}
			ImGui::Separator();
		}
		if (ImGui::MenuItem("Create Empty"))
		{
			auto object = scene->Instantiate();
			object->GetComponent<Transform>()->SetParent(transform);
			object->SetName("Empty Object");
		}
		if (ImGui::BeginMenu("Create 3D"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				GameObject* object = scene->Instantiate();
				object->GetComponent<Transform>()->SetParent(transform);
				object->SetName("Cube");

				Geometry geometry;
				Geometry::CreateCube(geometry);
				Mesh* mesh = new Mesh(mContext);
				mesh->Create(geometry);
				MeshRenderer* renderer = object->AddComponent<MeshRenderer>();
				renderer->SetMesh(mesh);
				renderer->SetMaterial(new Material(mContext));
			}
			if (ImGui::MenuItem("Sphere"))
			{
				GameObject* object = scene->Instantiate();
				object->GetComponent<Transform>()->SetParent(transform);
				object->SetName("Sphere");

				Geometry geometry;
				Geometry::CreateSphere(geometry);
				Mesh* mesh = new Mesh(mContext);
				mesh->Create(geometry);
				MeshRenderer* renderer = object->AddComponent<MeshRenderer>();
				renderer->SetMesh(mesh);
				renderer->SetMaterial(new Material(mContext));
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create Light"))
		{
			if (ImGui::MenuItem("Directional Light"))
			{
				GameObject* object = scene->Instantiate();
				object->GetComponent<Transform>()->SetParent(transform);
				object->SetName("Directional Light");

				object->AddComponent<Light>()->SetLightType(Light::Directional);
			}
			if (ImGui::MenuItem("Point Light"))
			{
				GameObject* object = scene->Instantiate();
				object->GetComponent<Transform>()->SetParent(transform);
				object->SetName("Point Light");

				object->AddComponent<Light>()->SetLightType(Light::Point);
			}
			if (ImGui::MenuItem("Spot Light"))
			{
				GameObject* object = scene->Instantiate();
				object->GetComponent<Transform>()->SetParent(transform);
				object->SetName("Spot Light");

				object->AddComponent<Light>()->SetLightType(Light::Spot);
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}