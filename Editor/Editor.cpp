#include "stdafx.h"
#include "Editor.h"
#include "../Framework/Engine.h"

#include "./ImGui/imgui_impl_win32.h"
#include "./ImGui/imgui_impl_dx11.h"

#include "./Widget/SceneWidget.h"
#include "./Widget/HierarchyWidget.h"
#include "./Widget/InspectorWidget.h"
#include "./Widget/LogWidget.h"
#include "./Widget/AssetWidget.h"

#define DOCKING_ENABLED ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

Editor::Editor()
{
	//Initialize engine
	m_engine = new Engine;
	m_context = m_engine->GetContext();
	m_graphics = m_context->GetSubsystem<Graphics>();
	Window* window = m_context->GetSubsystem<Window>();

	//Create ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//Initialize ImGui configuration flags
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigResizeWindowsFromEdges = true;

	//Initialize ImGui Win32/DX11 implementation
	ImGui_ImplWin32_Init(window->GetHandle());
	ImGui_ImplDX11_Init(m_graphics->GetDevice(), m_graphics->GetDeviceContext());

	//Initialize ImGui style
	ImGui::StyleColorsDark();

	//Add widgets
	m_widgets.emplace_back(new SceneWidget(m_context));
	m_widgets.emplace_back(new HierarchyWidget(m_context));
	m_widgets.emplace_back(new InspectorWidget(m_context));
	m_widgets.emplace_back(new LogWidget(m_context));
	m_widgets.emplace_back(new AssetWidget(m_context));

	//Attach resize and message handler to window
	window->AttachResizeProc([this](const UINT& width, const UINT& height) { Resize(width, height); });
	window->AttachMessageProc(ImGui_ImplWin32_WndProcHandler);
}

Editor::~Editor()
{
	//Destroy widgets
	for (auto& widget : m_widgets)
		if (widget) delete widget;

	//Shutdown Win32/DX11 implementation
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
	{
		//Start ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		BeginDockspace();
		{
			//Render widgets
			for (auto& widget : m_widgets)
			{
				widget->Begin();
				widget->Render();
				widget->End();
			}
		}
		EndDockspace();

		//Assemble and render draw data
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	m_graphics->End();
}

void Editor::BeginDockspace()
{
	//Initialize dockspace flags
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	//Initialize dockspace viewport
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);

	//Begin dockspace
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Dockspace", &m_dockspace, flags);
	ImGui::PopStyleVar(3);

	//Build dockspace
	ImGuiID id = ImGui::GetID("Dockspace");
	if (!ImGui::DockBuilderGetNode(id)) 
	{
		ImGui::DockBuilderAddNode(id, ImGui::GetMainViewport()->Size);
		ImGuiID main  = id;
		ImGuiID right = ImGui::DockBuilderSplitNode(main, ImGuiDir_Right, 0.25f, nullptr, &main);
		ImGuiID down  = ImGui::DockBuilderSplitNode(main, ImGuiDir_Down, 0.25f, nullptr, &main);
		ImGui::DockBuilderDockWindow("Scene", main);
		ImGui::DockBuilderDockWindow("Inspector", right);
		ImGui::DockBuilderDockWindow("Hierarchy", right);
		ImGui::DockBuilderDockWindow("Log", down);
		ImGui::DockBuilderDockWindow("Asset", down);
		ImGui::DockBuilderFinish(id);
	}
	ImGui::DockSpace(id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruDockspace);
}

void Editor::EndDockspace()
{
	ImGui::End();
}

void Editor::Resize(const UINT & width, const UINT & height)
{
	m_graphics->SetResolution(width, height);
}
