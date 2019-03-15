#include "Framework.h"
#include "SceneManager.h"

SceneManager::SceneManager(Context * context) :
	Subsystem<SceneManager>(context)
{
	m_currentScene = new Scene(context);
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	m_currentScene->Update();
}