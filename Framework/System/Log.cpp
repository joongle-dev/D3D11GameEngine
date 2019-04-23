#include "Framework.h"
#include "Log.h"
#include <iostream>

std::mutex       Log::sMutex;
Log::LogFunction Log::sLogFunc;

void Log::Write(const LogType type, const char * text, ...)
{
	static const char* typenames[] = { "Info", "Warning", "Error" };

	char buffer[1024];
	va_list args;
	va_start(args, text);
	vsnprintf_s(buffer, sizeof(buffer), text, args);
	va_end(args);

	std::lock_guard lock(sMutex);

	if (sLogFunc)
		sLogFunc(type, buffer);
	else
		printf("[%s]: %s\n", typenames[type], buffer);
}