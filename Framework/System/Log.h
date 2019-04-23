#pragma once
#include "Framework.h"

enum class LogType : unsigned char
{
	Info,
	Warning,
	Error,
};

class Log final
{
public:
	static void Write(const LogType type, const char* text)
	{
		std::lock_guard(sMutex);

		if (sFunc)
			sFunc(type, text);
		else
			printf("[%s]: %s\n", sTypenames[static_cast<unsigned char>(type)], text);
	}

private:
	Log() = default;
	~Log() = default;

	static std::mutex sMutex;
	static std::function<void(const LogType, const char*)> sFunc;
	static const char* sTypenames[];
};

const char* Log::sTypenames[] = { "Info", "Warning", "Error" };