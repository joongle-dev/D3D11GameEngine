#include "stdafx.h"

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int iCmdShow)
{
	//FbxLoader loader;
	//loader.Load("Boxing.fbx");

	Engine engine;

	Context* context = engine.GetContext();

	context->GetSubsystem<Graphics>()->SetClearColor(0, 0, 0, 1);
	context->GetSubsystem<Time>()->LockFramerate(144);

	ResourceManager* resources = context->GetSubsystem<ResourceManager>();
	resources->Load<Mesh>("Boy01_Body_Geo.mesh");

	Scene* scene = context->GetSubsystem<SceneManager>()->m_currentScene;
	GameObject* object = scene->Instantiate();
	object->AddComponent<Camera>();

	object = scene->Instantiate();
	MeshRenderer* meshRenderer = object->AddComponent<MeshRenderer>();
	meshRenderer->SetMesh(resources->Get<Mesh>("Boy01_Body_Geo"));

	engine.Run();

	return 0;
}