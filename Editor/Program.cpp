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

	//Importer importer(context);
	//importer.ImportModel("../Assets/Slash.fbx");

	context->GetSubsystem<Graphics>()->SetClearColor(Color(0, 0, 0, 1));
	//context->GetSubsystem<Time>()->LockFramerate(300);

	ResourceManager* resources = context->GetSubsystem<ResourceManager>();

	Scene* scene = context->GetSubsystem<SceneManager>()->GetCurrentScene();

	Geometry geometry;
	Geometry::CreateCube(geometry);
	geometry.GenerateTangents();

	Mesh* sphere = new Mesh(context);
	sphere->Create(geometry);

	//GameObject* object = scene->Instantiate();
	//object->SetName("T Pose");
	//MeshRenderer* meshrender = object->AddComponent<MeshRenderer>();
	////meshrender->SetMesh(resources->Load<Mesh>("Boy01_Body_Geo.mesh"));
	////meshrender->SetMaterial(resources->Load<Material>("Boy01_Body_MAT.mat"));
	//meshrender->SetMesh(resources->Load<Mesh>("Paladin_J_Nordstrom.mesh"));
	//meshrender->SetMaterial(resources->Load<Material>("Paladin_MAT.mat"));

	Material* material = new Material(context);
	material->SetTexture(Material::TEX_ALBEDO, resources->Load<Texture>("green-ceramic-tiles_basecolor.png"));
	material->SetTexture(Material::TEX_NORMAL, resources->Load<Texture>("green-ceramic-tiles_normal-dx.png"));
	//material->SetTexture(Material::TEX_NORMAL, resources->Load<Texture>("Untitled.png"));
	material->SetTexture(Material::TEX_HEIGHT, resources->Load<Texture>("green-ceramic-tiles_Height.png"));
	//material->SetTexture(Material::TEX_HEIGHT, resources->Load<Texture>("Untitled1.png"));
	material->SetTexture(Material::TEX_ROUGHNESS, resources->Load<Texture>("green-ceramic-tiles_roughness.png"));
	material->SetTexture(Material::TEX_METALLIC, resources->Load<Texture>("green-ceramic-tiles_metallic.png"));

	GameObject* object0 = scene->Instantiate();
	object0->SetName("Tile");
	//object0->GetComponent<Transform>()->SetScale(Vector3(10, 10, 10));
	MeshRenderer* meshrender0 = object0->AddComponent<MeshRenderer>();
	meshrender0->SetMesh(sphere);
	meshrender0->SetMaterial(material);

	material = new Material(context);
	material->SetTexture(Material::TEX_ALBEDO, resources->Load<Texture>("bark1-albedo.png"));
	material->SetTexture(Material::TEX_NORMAL, resources->Load<Texture>("bark1-normal3.png"));
	material->SetTexture(Material::TEX_HEIGHT, resources->Load<Texture>("bark1-height2.png"));
	material->SetTexture(Material::TEX_ROUGHNESS, resources->Load<Texture>("bark1-rough.png"));
	material->SetTexture(Material::TEX_METALLIC, resources->Load<Texture>("bark1-metalness.png"));

	object0 = scene->Instantiate();
	object0->SetName("Bark");
	//object0->GetComponent<Transform>()->SetScale(Vector3(10, 10, 10));
	meshrender0 = object0->AddComponent<MeshRenderer>();
	meshrender0->SetMesh(sphere);
	meshrender0->SetMaterial(material);

	editor.Run();

	return 0;
}