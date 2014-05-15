#include "WinUtils.h"

#include <Windows.h>

#include "Log.h"

namespace WinUtils {

bool TryCreateFolder(const std::string& path)
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

void GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames)
{
	WIN32_FIND_DATA findData;
	HANDLE findHandle = FindFirstFile((path + "\\*").c_str(), &findData);
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

bool TryCopyFile(const std::string& sourcePath, const std::string& destPath)
{
	BOOL success = ::CopyFile(sourcePath.c_str(), destPath.c_str(), FALSE);
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
	DWORD attributes = GetFileAttributes(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::string GetLastWindowsError()
{
	DWORD errorCode = ::GetLastError();
	const DWORD errorBufferSize = 256;
	CHAR errorBuffer[errorBufferSize];
	BOOL success = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
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
		return "Unknown error";
	}
}

} // namespace WinUtils
