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



#include "OSCompatibilityLayer.h"
#include <Windows.h>
#include <iostream>
#include <Shellapi.h>
#include "Log.h"



namespace Utils
{



bool TryCreateFolder(const std::string& path)
{
	BOOL success = ::CreateDirectoryW(convertToUTF16(path).c_str(), NULL);
	if (success || GetLastError() == 183)	// 183 is if the folder already exists
	{
		return true;
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not create folder " << path << " - " << GetLastErrorString();
		return false;
	}
}


std::string getCurrentDirectory()
{
	wchar_t curDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curDir);
	return Utils::convertToUTF8(curDir);
}


void GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames)
{
	WIN32_FIND_DATA findData;	// the structure to hold the file data
	HANDLE findHandle = FindFirstFileW(convertToUTF16(path + "/*").c_str(), &findData);	// the results of the file search
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			fileNames.insert(convertToUTF8(findData.cFileName));
		}
	} while (FindNextFileW(findHandle, &findData) != 0);
	FindClose(findHandle);
}


bool TryCopyFile(const std::string& sourcePath, const std::string& destPath)
{
	BOOL success = ::CopyFileW(convertToUTF16(sourcePath).c_str(), convertToUTF16(destPath).c_str(), FALSE);	// whether or not the copy succeeded
	if (success)
	{
		return true;
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not copy file " << sourcePath << " to " << destPath << " - " << GetLastErrorString();
		return false;
	}
}


bool copyFolder(const std::string& sourceFolder, const std::string& destFolder)
{
	std::wstring wideSource = convertToUTF16(sourceFolder);
	wchar_t* from = new wchar_t[wideSource.size() + 2];
	wcscpy(from, wideSource.c_str());
	from[wideSource.size() + 1] = '\0';

	std::wstring wideDest = convertToUTF16(destFolder);
	wchar_t* to = new wchar_t[wideDest.size() + 1];
	wcscpy(to, wideDest.c_str());
	to[wideDest.size() + 1] = '\0';

	SHFILEOPSTRUCT fileOptStruct;
	fileOptStruct.hwnd	= NULL;
	fileOptStruct.wFunc	= FO_COPY;
	fileOptStruct.pFrom	= from;
	fileOptStruct.pTo		= to;
	fileOptStruct.fFlags	= FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

	int result = SHFileOperation(&fileOptStruct);
	if (result != 0)
	{
		LOG(LogLevel::Error) << "Could not copy" << sourceFolder << " to " << destFolder << ". Error code: " << result;
	}
	else if (fileOptStruct.fAnyOperationsAborted)
	{
		LOG(LogLevel::Error) << "Could not copy" << sourceFolder << " to " << destFolder << ". Operation aborted";
	}

	delete[] from;
	delete[] to;
	return true;
}


bool renameFolder(const std::string& sourceFolder, const std::string& destFolder)
{
	std::wstring wideSource = convertToUTF16(sourceFolder);
	wchar_t* from = new wchar_t[wideSource.size() + 2];
	wcscpy(from, wideSource.c_str());
	from[wideSource.size() + 1] = '\0';

	std::wstring wideDest = convertToUTF16(destFolder);
	wchar_t* to = new wchar_t[wideDest.size() + 1];
	wcscpy(to, wideDest.c_str());
	to[wideDest.size() + 1] = '\0';

	SHFILEOPSTRUCT fileOptStruct;
	fileOptStruct.hwnd	= NULL;
	fileOptStruct.wFunc	= FO_MOVE;
	fileOptStruct.pFrom	= from;
	fileOptStruct.pTo		= to;
	fileOptStruct.fFlags	= FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;

	int result = SHFileOperation(&fileOptStruct);
	if (result != 0)
	{
		LOG(LogLevel::Error) << "Could not rename" << sourceFolder << " to " << destFolder << ". Error code: " << result;
	}
	else if (fileOptStruct.fAnyOperationsAborted)
	{
		LOG(LogLevel::Error) << "Could not rename" << sourceFolder << " to " << destFolder << ". Operation aborted";
	}

	delete[] from;
	delete[] to;
	return true;
}


bool DoesFileExist(const std::string& path)
{
	DWORD attributes = GetFileAttributesW(convertToUTF16(path).c_str());	// the file attributes
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}


bool doesFolderExist(const std::string& path)
{
	DWORD attributes = GetFileAttributesW(convertToUTF16(path).c_str());	// the file attributes
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}


std::string GetLastErrorString()
{
	DWORD errorCode = ::GetLastError();	// the code for the latest error
	const DWORD errorBufferSize = 256;	// the size of the textbuffer for the error
	wchar_t errorBuffer[errorBufferSize];	// the text buffer for the error
	BOOL success = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,		// whether or not the error could be formatted
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		errorBuffer,
		errorBufferSize - 1,
		NULL);
	if (success)
	{
		return convertToUTF8(errorBuffer);
	}
	else
	{
		return "Unknown error";
	}
}


std::string convertToASCII(std::string UTF8)
{
	char asciiArray[1024];
	if (0 == WideCharToMultiByte(20127 /*US-ASCII (7-bit)*/, 0, convertToUTF16(UTF8).c_str(), -1, asciiArray, 1024, "0", NULL))
	{
		LOG(LogLevel::Error) << "Could not translate string to ASCII - " << GetLastErrorString();
	}
	std::string returnable(asciiArray);

	return returnable;
}


std::string convertToUTF8(std::wstring UTF16)
{
	int requiredSize = WideCharToMultiByte(CP_UTF8, 0, UTF16.c_str(), -1, NULL, 0, NULL, NULL);
	char* utf8array = new char[requiredSize];

	if (0 == WideCharToMultiByte(CP_UTF8, 0, UTF16.c_str(), -1, utf8array, requiredSize, NULL, NULL))
	{
		LOG(LogLevel::Error) << "Could not translate string to UTF-8 - " << GetLastErrorString();
	}
	std::string returnable(utf8array);

	delete[] utf8array;

	return returnable;
}


std::string convert8859_15ToUTF8(std::string input)
{
	return convertToUTF8(convertToUTF16(input));
}


std::wstring convertToUTF16(std::string UTF8)
{
	int requiredSize = MultiByteToWideChar(28605 /* 8859-15*/, MB_PRECOMPOSED, UTF8.c_str(), -1, NULL, 0);
	wchar_t* wideKeyArray = new wchar_t[requiredSize];

	if (0 == MultiByteToWideChar(28605 /* 8859-15*/, MB_PRECOMPOSED, UTF8.c_str(), -1, wideKeyArray, requiredSize))
	{
		LOG(LogLevel::Error) << "Could not translate string to UTF-16 - " << GetLastErrorString();
	}
	std::wstring returnable(wideKeyArray);

	delete[] wideKeyArray;

	return returnable;
}


void WriteToConsole(LogLevel level, const std::string& logMessage)
{
	if (level == LogLevel::Debug)
	{	// Don't log debug messages to console.
		return;
	}

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);	// a handle to the console window
	if (console != INVALID_HANDLE_VALUE)
	{
		CONSOLE_SCREEN_BUFFER_INFO oldConsoleInfo;	// the current (soon to be outdated) console data
		BOOL success = GetConsoleScreenBufferInfo(console, &oldConsoleInfo);	// whether or not the console data could be retrieved
		if (success)
		{
			WORD color;	// the color the text will be
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
			default:
				color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				break;
			}
			SetConsoleTextAttribute(console, color);
			DWORD bytesWritten = 0;
			WriteConsoleW(console, Utils::convertToUTF16(logMessage).c_str(), logMessage.size(), &bytesWritten, NULL);

			// Restore old console color.
			SetConsoleTextAttribute(console, oldConsoleInfo.wAttributes);

			return;
		}
	}

	std::cout << logMessage;
}



} // namespace Utils
