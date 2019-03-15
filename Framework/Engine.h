#pragma once

class Engine
{
public:
	Engine();
	~Engine() = default;

	void Update();

	class Context* GetContext() const { return m_context.get(); }

protected:
	std::unique_ptr<class Context> m_context;
};