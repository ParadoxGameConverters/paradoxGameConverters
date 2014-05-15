#include "Log.h"

#include <ctime>
#include <fstream>
#include <iostream>

#include <Windows.h>

Log::Log(LogLevel level)
: logLevel(level)
{
	static bool logFileCreated = false;
	if (!logFileCreated)
	{
		std::ofstream logFile("log.txt", std::ofstream::trunc);
		logFileCreated = true;
	}
}

Log::~Log()
{
	logMessageStream << std::endl;
	std::string logMessage = logMessageStream.str();
	WriteToConsole(logLevel, logMessage);
	WriteToFile(logLevel, logMessage);
}

void Log::WriteToConsole(LogLevel level, const std::string& logMessage)
{
	if (level == LogLevel::Debug)
	{	// Don't log debug messages to console.
		return;
	}

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console != INVALID_HANDLE_VALUE)
	{
		CONSOLE_SCREEN_BUFFER_INFO oldConsoleInfo;
		BOOL success = GetConsoleScreenBufferInfo(console, &oldConsoleInfo);
		if (success)
		{
			WORD color;
			switch (level)
			{
				case LogLevel::Error:
					color = FOREGROUND_RED | FOREGROUND_INTENSITY;
					break;

				case LogLevel::Warning:
					color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					break;

				case LogLevel::Info:
					color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					break;

				case LogLevel::Debug:
					color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
					break;
			}
			SetConsoleTextAttribute(console, color);
			DWORD bytesWritten = 0;
			WriteConsoleA(console, logMessage.c_str(), logMessage.size(), &bytesWritten, NULL);

			// Restore old console color.
			SetConsoleTextAttribute(console, oldConsoleInfo.wAttributes);

			return;
		}
	}

	std::cout << logMessage;
}

void Log::WriteToFile(LogLevel level, const std::string& logMessage)
{
	std::ofstream logFile("log.txt", std::ofstream::app);

	time_t rawtime;
	time(&rawtime);
	tm timeInfo;
	errno_t error = localtime_s(&timeInfo, &rawtime);
	if (error == 0)
	{
		char timeBuffer[64];
		size_t bytesWritten = strftime(timeBuffer, 64, "%Y-%m-%d %H:%M:%S ", &timeInfo);
		if (bytesWritten != 0)
		{
			logFile << timeBuffer;
		}
	}

	switch (level)
	{
		case LogLevel::Error:
			logFile << "  [ERROR] ";
			break;

		case LogLevel::Warning:
			logFile << "[WARNING] ";
			break;

		case LogLevel::Info:
			logFile << "   [INFO] ";
			break;

		case LogLevel::Debug:
			logFile << "  [DEBUG]     ";	// Debug messages are extra indented to further de-emphasize them.
			break;
	}
	logFile << logMessage;
}
