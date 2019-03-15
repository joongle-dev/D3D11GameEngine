#pragma once
#include "Subsystem.h"

class Scene;
class SceneManager final : public Subsystem<SceneManager>
{
public:
	SceneManager(class Context* context);
	~SceneManager();

	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	void Update();

public:
	Scene* m_currentScene;
};