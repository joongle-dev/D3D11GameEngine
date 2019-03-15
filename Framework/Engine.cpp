#include "Framework.h"
#include "Engine.h"


Engine::Engine()
{
	m_context = std::make_unique<Context>();
	m_context->AddSubsystem<Time>();
	m_context->AddSubsystem<Input>();
	m_context->AddSubsystem<Window>(L"Game Engine", 1280, 720);
	m_context->AddSubsystem<Graphics>();
	m_context->AddSubsystem<ResourceManager>();
	m_context->AddSubsystem<ComponentManager>();
	m_context->AddSubsystem<SceneManager>();
	m_context->AddSubsystem<Renderer>();
}

void Engine::Update()
{
	//m_context->Update();
}