/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include "OSCompatibilityLayer.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>



#pragma warning(disable : 4996)	// suppress warning about localtime, asthe alternative is windows-specific



Log::Log(LogLevel level)
: logLevel(level)
{
	static bool logFileCreated = false;	// whether or not the log file has been created this run of the converter
	if (!logFileCreated)
	{
		std::ofstream logFile("log.txt", std::ofstream::trunc);
		logFileCreated = true;
		logFile.close();
	}
}


Log::~Log()
{
	logMessageStream << std::endl;
	std::string logMessage = logMessageStream.str();
	Utils::WriteToConsole(logLevel, logMessage);
	WriteToFile(logLevel, logMessage);
}


static std::map<LogLevel, std::string> logLevelStrings = {
	{LogLevel::Error, "  [ERROR] "},
	{LogLevel::Warning, "[WARNING] "},
	{LogLevel::Info, "   [INFO] "},
	{LogLevel::Debug, "  [DEBUG]     "}
};
void Log::WriteToFile(LogLevel level, const std::string& logMessage)
{
	std::ofstream logFile("log.txt", std::ofstream::app);
	WriteTheTime(logFile);
	logFile << logLevelStrings[level];
	logFile << logMessage;
}


void Log::WriteTheTime(std::ofstream& logFile)
{
	time_t rawtime;
	time(&rawtime);
	const tm* timeInfo = localtime(&rawtime);
	char timeBuffer[64];
	const size_t bytesWritten = strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S ", timeInfo);
	if (bytesWritten != 0)
	{
		logFile << timeBuffer;
	}
}
