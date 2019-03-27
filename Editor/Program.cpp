#include "stdafx.h"
#include "Editor.h"
#include "Importer/Loader.h"

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int iCmdShow)
{

	Editor editor;

	Context* context = editor.GetContext();

	Importer importer(context);
	importer.ImportModel("../Assets/Boxing.fbx");

	context->GetSubsystem<Graphics>()->SetClearColor(Color(0, 0, 0, 1));
	//context->GetSubsystem<Time>()->LockFramerate(300);

	ResourceManager* resources = context->GetSubsystem<ResourceManager>();

	Scene* scene = context->GetSubsystem<SceneManager>()->GetCurrentScene();

	GameObject* object = scene->Instantiate();
	object->SetName("T Pose");
	MeshRenderer* meshrender = object->AddComponent<MeshRenderer>();
	meshrender->SetMesh(resources->Load<Mesh>("Boy01_Body_Geo.mesh"));
	meshrender->SetMaterial(resources->Load<Material>("Boy01_Body_MAT.mat"));

	GameObject* object1 = scene->Instantiate();
	object1->GetComponent<Transform>()->SetParent(object->GetComponent<Transform>());

	editor.Run();

	return 0;
}