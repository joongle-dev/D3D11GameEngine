#pragma once
#include "Subsystem.h"

class SceneManager final : public Subsystem<SceneManager>
{
public:
	SceneManager(class Context* context);
	~SceneManager();

	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	void Update();

	class Scene* GetCurrentScene() const { return m_current; }

private:
	class Scene* m_current;
};