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



#include "WinUtils.h"
#include <Windows.h>
#include "Log.h"



namespace WinUtils {



bool TryCreateFolder(const std::string& path)
{
	BOOL success = ::CreateDirectoryW(convertToUTF16(path).c_str(), NULL);
	if (success || GetLastError() == 183)	// 183 is if the folder already exists
	{
		return true;
	}
	else
	{
		LOG(LogLevel::Warning) << "Could not create folder " << path << " - " << GetLastWindowsError();
		return false;
	}
}


void GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames)
{
	WIN32_FIND_DATA findData;	// the structure to hold the file data
	HANDLE findHandle = FindFirstFileW(convertToUTF16(path + "\\*").c_str(), &findData);	// the results of the file search
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
		LOG(LogLevel::Warning) << "Could not copy file " << sourcePath << " to " << destPath << " - " << GetLastWindowsError();
		return false;
	}
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


std::string GetLastWindowsError()
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
	if (0 == WideCharToMultiByte(20127 /*US-ASCII (7-bit)*/, 0, convertToUTF16(UTF8).c_str(), -1, asciiArray, 1024, NULL, NULL))
	{
		LOG(LogLevel::Error) << "Could not translate string to ASCII - " << GetLastWindowsError();
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
		LOG(LogLevel::Error) << "Could not translate string to UTF-8 - " << GetLastWindowsError();
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
		LOG(LogLevel::Error) << "Could not translate string to UTF-16 - " << GetLastWindowsError();
	}
	std::wstring returnable(wideKeyArray);

	delete[] wideKeyArray;

	return returnable;
}



} // namespace WinUtils
