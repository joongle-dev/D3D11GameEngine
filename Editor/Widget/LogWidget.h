#pragma once
#include "IWidget.h"

class LogWidget final : public IWidget
{
	struct LogEntry
	{
		LogType type;
		std::string text;

		LogEntry(LogType type, const std::string& text) :
			type(type), text(text) {}
	};

public:
	LogWidget(class Context* context);
	~LogWidget() = delete;

	void Render() override;

	void AddLog(LogType type, const char* text);

private:
	bool mShowInfo;
	bool mShowWarning;
	bool mShowError;

	std::vector<LogEntry> mLogs;
	std::vector<Texture*> mIcons;
};