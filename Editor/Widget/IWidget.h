#pragma once
#include "stdafx.h"

class IWidget
{
public:
	IWidget(class Context* context) :
		mContext(context),
		mName(""),
		mIsVisible(true),
		mWindowFlags(ImGuiWindowFlags_NoCollapse)
	{}
	~IWidget() = default;

	virtual void Begin()
	{
		ImGui::Begin(mName.c_str(), &mIsVisible, mWindowFlags);
	}

	virtual void Render() = 0;

	virtual void End()
	{
		ImGui::End();
	}

protected:
	class Context* mContext;

	std::string mName;
	unsigned int mWindowFlags;
	bool mIsVisible;
};