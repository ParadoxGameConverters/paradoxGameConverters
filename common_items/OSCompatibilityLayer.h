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



#ifndef OS_COMPATABILITY_LAYER_H
#define OS_COMPATABILITY_LAYER_H



#include <cstdio>
#include <stdint.h>
#include <set>
#include <string>
#include "Log.h"



#ifndef MAX_PATH
#define MAX_PATH 260
#endif //MAX_PATH



//Linux specific defines
#ifdef __linux

//sprintf_s is not implemented on Linux. It functions similarly to the function snprintf, but the return values are different.
//Since return values are not used, they are directly translated for now. If return values will some day be used, the compiler
//will give an error due to sprintf_s_Linux returning void.
#define sprintf_s sprintf_s_Linux
void sprintf_s_Linux (char *__restrict __s, size_t __maxlen, const char *__restrict __format, ...);

//See sprintf_s_LInux
#define strcpy_s strcpy_s_Linux
void strcpy_s_Linux(char *__restrict __dest, const char *__restrict __src);

//Very basic implementation, simply returns 0 if FILE* is not NULL
#define fopen_s fopen_s_Linux
int fopen_s_Linux(FILE** file, const char* filename, const char* mode);

#define fprintf_s fprintf_s_Linux
void fprintf_s_Linux(FILE* file, const char* format, ...);

typedef uint64_t HANDLE;
#define STD_OUTPUT_HANDLE -11
#define INVALID_HANDLE_VALUE -1
HANDLE GetStdHandle(int nothing);

typedef int errno_t;

#endif //__linux__



namespace Utils
{

	// Creates a new folder corresponding to the given path.
	// Returns true on success or if the folder already exists.
	// Returns false and logs a warning on failure.
	bool TryCreateFolder(const std::string& path);
	std::string getCurrentDirectory();
	// Adds all files (just the file name) in the specified folder to the given collection.
	void GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames);

	// For the specified folder and all subfolders, adds all files (just the subdirectory and file name)
	// to the given collection.
	void GetAllFilesInFolderRecursive(const std::string& path, std::set<std::string>& filenames);
	// Copies the file specified by sourcePath as destPath.
	// Returns true on success.
	// Returns false and logs a warning on failure.
	bool TryCopyFile(const std::string& sourcePath, const std::string& destPath);
	bool copyFolder(const std::string& sourceFolder, const std::string& destFolder);
	bool renameFolder(const std::string& sourceFolder, const std::string& destFolder);
	// Returns true if the specified file exists (and is a file rather than a folder).
	bool DoesFileExist(const std::string& path);
	// Returns true if the specified folder exists (and is a folder rather than a file).
	bool doesFolderExist(const std::string& path);

	void WriteToConsole(LogLevel level, const std::string& logMessage);

	// Returns a formatted string describing the last error on the WinAPI.
	std::string GetLastErrorString();

	bool deleteFolder(const std::string& folder);


	std::string GetFileFromTag(const std::string& directoryPath, const std::string& tag);

	std::string convertUTF8ToASCII(std::string UTF8);
	std::string convertUTF8To8859_15(std::string UTF8);
	std::string convertUTF16ToUTF8(std::wstring UTF16);
	std::string convert8859_15ToUTF8(std::string input);
	std::wstring convert8859_15ToUTF16(std::string UTF8);
	std::wstring convertUTF8ToUTF16(std::string UTF8);
} // namespace Utils


#endif //OS_COMPATABILITY_LAYER_H
