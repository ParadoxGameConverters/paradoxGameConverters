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



bool TryCreateFolder(const std::wstring& path)
{
	BOOL success = ::CreateDirectory(path.c_str(), NULL);
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

void GetAllFilesInFolder(const std::wstring& path, std::set<std::wstring>& fileNames)
{
	WIN32_FIND_DATA findData;	// the structure to hold the file data
	HANDLE findHandle = FindFirstFile((path + L"\\*").c_str(), &findData);	// the results of the file search
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			fileNames.insert(findData.cFileName);
		}
	} while (FindNextFile(findHandle, &findData) != 0);
	FindClose(findHandle);
}


bool TryCopyFile(const std::wstring& sourcePath, const std::wstring& destPath)
{
	BOOL success = ::CopyFile(sourcePath.c_str(), destPath.c_str(), FALSE);	// whether or not the copy succeeded
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


bool DoesFileExist(const std::wstring& path)
{
	DWORD attributes = GetFileAttributes(path.c_str());	// the file attributes
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}


bool doesFolderExist(const std::wstring& path)
{
	DWORD attributes = GetFileAttributes(path.c_str());	// the file attributes
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}


std::wstring GetLastWindowsError()
{
	DWORD errorCode = ::GetLastError();	// the code for the latest error
	const DWORD errorBufferSize = 256;	// the size of the textbuffer for the error
	wchar_t errorBuffer[errorBufferSize];	// the text buffer for the error
	BOOL success = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,		// whether or not the error could be formatted
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		errorBuffer,
		errorBufferSize - 1,
		NULL);
	if (success)
	{
		return errorBuffer;
	}
	else
	{
		return L"Unknown error";
	}
}


std::string convertToASCII(std::wstring UTF16)
{
	char utf8array[1024];
	if (0 == WideCharToMultiByte(20127 /*US-ASCII (7-bit)*/, 0, UTF16.c_str(), -1, utf8array, 1024, NULL, NULL))
	{
		int errorCode = GetLastError();
		LOG(LogLevel::Error) << "Could not translate string to UTF-8. Error code " <<errorCode;
	}
	std::string returnable(utf8array);

	return returnable;
}


std::string convertToUTF8(std::wstring UTF16)
{
	char utf8array[1024];
	if (0 == WideCharToMultiByte(CP_UTF8, 0, UTF16.c_str(), -1, utf8array, 1024, NULL, NULL))
	{
		int errorCode = GetLastError();
		LOG(LogLevel::Error) << "Could not translate string to UTF-8. Error code " <<errorCode;
	}
	std::string returnable(utf8array);

	return returnable;
}


std::wstring convertToUTF16(std::string UTF8)
{
	wchar_t wideKeyArray[1024];
	if (0 == MultiByteToWideChar(28605 /* 8859-15*/, MB_PRECOMPOSED, UTF8.c_str(), -1, wideKeyArray, 1024))
	{
		int errorCode = GetLastError();
		LOG(LogLevel::Error) << "Could not translate string to UTF-16. Error code " <<errorCode;
	}
	std::wstring returnable(wideKeyArray);

	return returnable;
}



} // namespace WinUtils
