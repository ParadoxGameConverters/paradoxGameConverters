#ifndef LOG_H_
#define LOG_H_

#include <sstream>
#include <string>

#define LOG(LOG_LEVEL) Log(LOG_LEVEL)

enum class LogLevel
{
	Error,
	Warning,
	Info,
	Debug
};

class Log
{
public:
	Log(LogLevel);
	~Log();

	template<class T>
	Log& operator<<(T t)
	{
		logMessageStream << t;
		return *this;
	}

private:
	static void WriteToConsole(LogLevel, const std::string& logMessage);
	static void WriteToFile(LogLevel, const std::string& logMessage);

	LogLevel logLevel;
	std::ostringstream logMessageStream;
};

#endif // LOG_H_