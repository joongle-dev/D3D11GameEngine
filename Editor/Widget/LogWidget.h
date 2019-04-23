#pragma once
#include "IWidget.h"

class LogWidget final : public IWidget
{
public:
	LogWidget(class Context* context);
	~LogWidget() = delete;

	void Render() override;

private:
	bool mShowInfo;
	bool mShowWarning;
	bool mShowError;
};