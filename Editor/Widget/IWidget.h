#pragma once
#include "stdafx.h"

class IWidget
{
public:
	IWidget(class Context* context) :
		m_context(context),
		m_name(""),
		m_visible(true),
		m_flags(ImGuiWindowFlags_NoCollapse)
	{}
	~IWidget() = default;

	virtual void Begin()
	{
		ImGui::Begin(m_name.c_str(), &m_visible, m_flags);
	}

	virtual void Render() = 0;

	virtual void End()
	{
		ImGui::End();
	}

protected:
	class Context* m_context;

	std::string m_name;
	int m_flags;
	bool m_visible;
};