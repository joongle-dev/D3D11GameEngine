#pragma once

class Editor final
{
public:
	Editor();
	~Editor();

	void Run();

	void Update();
	void Render();

	class Context* GetContext() const { return m_context; }

private:
	class Engine* m_engine;
	class Context* m_context;
	class Graphics* m_graphics;

	std::vector<class IWidget*> m_widgets;
};

