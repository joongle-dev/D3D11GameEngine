#pragma once

#define LOG_INFO(text, ...)    Log::Write(LogType_Info, text, __VA_ARGS__);
#define LOG_WARNING(text, ...) Log::Write(LogType_Warning, text, __VA_ARGS__);
#define LOG_ERROR(text, ...)   Log::Write(LogType_Error, text, __VA_ARGS__);

enum LogType : unsigned char
{
	LogType_Info,
	LogType_Warning,
	LogType_Error,
};

class Log final
{
	using LogFunction = std::function<void(const LogType, const char*)>;

public:
	static void Write(const LogType type, const char* text, ...);
	
	static void SetLogFunction(LogFunction func) { sLogFunc = func;	}
	static void SetLogFile(const std::string& path);

private:
	Log() = default;
	~Log() = default;

	static std::mutex   sMutex;
	static LogFunction  sLogFunc;
};