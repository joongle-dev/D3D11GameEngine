#include "stdafx.h"
#include "Editor.h"
#include "../Framework/Engine.h"

#include "./ImGui/imgui_impl_win32.h"
#include "./ImGui/imgui_impl_dx11.h"

#include "./Widget/Widget_Scene.h"
#include "./Widget/Widget_Hierarchy.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

Editor::Editor()
{
	m_engine = new Engine;
	
	m_context = m_engine->GetContext();
	m_graphics = m_context->GetSubsystem<Graphics>();
	
	Window* window = m_context->GetSubsystem<Window>();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(window->GetHandle());
	ImGui_ImplDX11_Init(m_graphics->GetDevice(), m_graphics->GetDeviceContext());
	ImGui::StyleColorsDark();

	window->WndProcSubscribe(ImGui_ImplWin32_WndProcHandler);

	m_widgets.emplace_back(new Widget_Scene(m_context));
	m_widgets.emplace_back(new Widget_Hierarchy(m_context));
}

Editor::~Editor()
{
	for (auto& widget : m_widgets)
		if (widget) delete widget;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void Editor::Run()
{
	while (m_context->IsActive()) {
		Update();
		Render();
	}
}

void Editor::Update()
{
	m_engine->Update();
}

void Editor::Render()
{
	m_graphics->Begin();

	//Start ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//Test window
	ImGui::Begin("Test Window");
	ImGui::Text("Frame time: %5.2fms", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::End();
	for (auto& widget : m_widgets)
	{
		widget->Begin();
		widget->Render();
		widget->End();
	}

	//Assemble and render draw data
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_graphics->End();
}
