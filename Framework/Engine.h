#pragma once

class Engine final
{
public:
	Engine();
	~Engine() = default;

	void Run();

	class Context* GetContext() const { return m_context.get(); }

private:
	std::unique_ptr<class Context> m_context;
	class Graphics* graphics;
	class Window* window;
};