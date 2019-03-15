#include "stdafx.h"
#include "Editor.h"

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int iCmdShow)
{
	//FbxLoader loader;
	//loader.Load("Boxing.fbx");

	Editor editor;

	Context* context = editor.GetContext();

	context->GetSubsystem<Graphics>()->SetClearColor(Color(0, 0, 0, 1));
	context->GetSubsystem<Time>()->LockFramerate(144);

	ResourceManager* resources = context->GetSubsystem<ResourceManager>();
	resources->Load<Mesh>("Boy01_Body_Geo.mesh");

	Scene* scene = context->GetSubsystem<SceneManager>()->GetCurrentScene();

	GameObject* object = scene->Instantiate();
	object->SetName("T Pose");
	object->AddComponent<MeshRenderer>()->SetMesh(resources->Get<Mesh>("Boy01_Body_Geo"));

	GameObject* object1 = scene->Instantiate();
	object1->GetComponent<Transform>()->SetParent(object->GetComponent<Transform>());

	editor.Run();

	return 0;
}