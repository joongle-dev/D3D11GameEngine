#include "Framework.h"
#include "SceneManager.h"

SceneManager::SceneManager(Context * context) :
	Subsystem<SceneManager>(context)
{
	m_current = new Scene(context);
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	m_current->Update();
}