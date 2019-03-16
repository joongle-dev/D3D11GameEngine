#pragma once

class Editor final
{
public:
	Editor();
	~Editor();

	void Run();

	void Update();
	void Render();

	void BeginDockspace();
	void EndDockspace();

	void Resize(const UINT& width, const UINT& height);

	class Context* GetContext() const { return m_context; }

private:
	class Engine* m_engine;
	class Context* m_context;
	class Graphics* m_graphics;

	std::vector<class IWidget*> m_widgets;
	bool m_dockspace;
};

