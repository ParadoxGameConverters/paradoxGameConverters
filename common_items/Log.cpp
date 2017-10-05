/*Copyright (c) 2016 The Paradox Game Converters Project

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


void Log::WriteToFile(LogLevel level, const std::string& logMessage)
{
	std::ofstream logFile("log.txt", std::ofstream::app);

	time_t rawtime;	// the raw time data
	time(&rawtime);
	tm* timeInfo = localtime(&rawtime); // the processed time data
	if (timeInfo) // whether or not there was an error
	{
		char timeBuffer[64];	// the formatted time
		size_t bytesWritten = strftime(timeBuffer, 64, "%Y-%m-%d %H:%M:%S ", timeInfo);	// the number of digits for the time stamp
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
