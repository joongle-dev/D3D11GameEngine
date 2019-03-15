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

void Engine::Run()
{
	Graphics* graphics = m_context->GetSubsystem<Graphics>();
	Time* time = m_context->GetSubsystem<Time>();
	
	static unsigned int count = 0;
	static float total = 0.0f;
	static float min = 1000.0f;
	static float max = 0.0f;

	while (m_context->IsActive())
	{

		graphics->Begin();
		m_context->Update();
		graphics->End();

		float elapsed = time->Elapsed() * 1000.0f;
		count++;
		total += elapsed;
		if (elapsed < min) min = elapsed;
		if (elapsed > max) max = elapsed;
		if (total >= 1000.0f) {
			printf("Frame time:  avg.%6.2fms  min.%6.2fms  max.%6.2fms\n", total / (float)count, min, max);
			total -= 1000.0f;
			min = 1000.0f;
			max = 0.0f;
			count = 0;
		}
	}
}