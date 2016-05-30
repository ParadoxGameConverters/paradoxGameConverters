/*Copyright (c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/


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
