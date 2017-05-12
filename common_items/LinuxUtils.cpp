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



#include "OSCompatibilityLayer.h"
#include "Log.h"
#include <cstdarg>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>

using namespace std;



void sprintf_s_Linux (char *__restrict __s, size_t __maxlen, const char *__restrict __format, ...)
{
	va_list argptr;
	va_start(argptr, __format);
	snprintf(__s, __maxlen, __format, argptr);
	va_end(argptr);
}


void strcpy_s_Linux(char *__restrict __dest, const char *__restrict __src)
{
	strcpy(__dest, __src);
}


int fopen_s_Linux(FILE** file, const char* filename, const char* mode)
{
	*file = fopen(filename, mode);
	return *file == NULL;
}


void fprintf_s_Linux(FILE* file, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	fprintf(file, format, argptr);
	va_end(argptr);
}


HANDLE GetStdHandle(int nothing)
{
	return 1;
}


namespace Utils
{
	
	bool TryCreateFolderNonRecursive(const char *path)
	{
        	const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH;
        	struct stat status;
        	if(stat(path, &status) != 0)
			{
                	if(mkdir(path, mode) != 0 && errno != EEXIST)
			{
                        	LOG(LogLevel::Error) << "unable to create folder: " << path << "\n";
                       		return false;
                	}
        	}else{
                	if(!S_ISDIR(status.st_mode))
			{
                        	LOG(LogLevel::Error) << "node already exists but is not a folder: " << path << "\n";
                        	return false;
                	}
        	}
        	return true;
	}


	char *CopyFolderPathElement(const char * &input_begin, const char *input_end, char *output)
	{
        	while(input_begin != input_end)
		{
                	const char c = *input_begin;
                	++input_begin;
                	if(c == '/')
			{
                        	return output;
                	}else{
                        	*output = c;
                        	++output;
                	}
        	}
        	return output;
	};

	bool TryCreateFolder(const std::string& path)
	{
		using namespace std;
        	const char *input_begin = path.c_str();
        	const char *input_end = input_begin+path.size();
        	char *buffer = new char[path.size()+1];
        	char *pos = buffer;
        	while(input_begin !=input_end)
		{
                	char *next_pos = CopyFolderPathElement(input_begin, input_end, pos);
                	if(next_pos != pos){
                        	*next_pos = '\0';
                        	if(!TryCreateFolderNonRecursive(buffer)){
                                	LOG(LogLevel::Error) << "unable to create folder for path " << buffer;
                                	delete[] buffer;
                                	return false;
                        	}
                	}
                	*next_pos = '/';
                	++next_pos;
                	pos = next_pos;
        	}
        	delete[] buffer;
        	return true;
	}

	std::string getCurrentDirectory()
	{
	        using namespace std;
       		char executable[PATH_MAX];
        	ssize_t length = readlink("/proc/self/exe", executable, PATH_MAX);
        	if(length == -1)
		{
                	LOG(LogLevel::Error) << "unable to fetch current directory";
                	return string("/");
        	}
       		char *begin = executable;
        	char *end = begin + length - 1;
        	while(end != begin && *end != '/')
		{
                	--end;
        	}
        	if(begin == end)
		{
                	return string(begin, begin + length);
        	}else{
                	return string(begin, end);
        	}
	}
	
	mode_t GetFileMode(const std::string &path)
	{
		struct stat status;
        	if(stat(path.c_str(), &status) != 0))
		{
			retrun status.st_mode;
		}else{
			LOG(LogLevel::Error) << "unable to check for file mode: " << path;
			return false;
		}
	}
	
	bool IsRegularFile(const std::string &path)
	{
		struct stat status;
        	if(stat(path.c_str(), &status) != 0))
		{
			retrun S_ISREG(status.st_mode);
		}else{
			LOG(LogLevel::Error) << "unable to check for regular file: " << path;
			return false;
		}
	}
	
	void GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames)
	{
		using namespace std;
		DIR *dir = opendir(path.c_str());
		if(dir == NULL)
		{
			if(errno == EACCES)
			{
				LOG(LogLevel::Error) << "no permission to read directory: " << path;
			}else if(errno == ENOENT)
			{
				LOG(LogLevel::Error) << "directory does not exist: " << path;
			}else if(errno == ENOTDIR)
			{
				LOG(LogLevel::Error) << "path is not a directory: " << path;
			}else{
				LOG(LogLevel::Error) << "unable to open directory: " << path;
			}
		}else{
			struct dirent *dirent_ptr;
			while((dirent_ptr = read_dir(dir)) != NULL){
				string fileName{dirent_ptr->name};
				if(IsRegularFile(path+fileName)){
					fileNames.insert(fileName);
				}
			}
			if(errno != 0){
				fileNames.clear();
				LOG(LogLevel::Error) << "an error occurred hile trying to list files in path: " << path;
			}
			closedir(dir);
		}
	}
	
	void GetAllFilesInFolderRecursive(const std::string& path, const std::string relativePath, std::set<std::string>& filenames)
	{
		using namespace std;
		DIR *dir = opendir(path.c_str());
		if(dir == NULL)
		{
			if(errno == EACCES)
			{
				LOG(LogLevel::Error) << "no permission to read directory: " << path;
			}else if(errno == ENOENT)
			{
				LOG(LogLevel::Error) << "directory does not exist: " << path;
			}else if(errno == ENOTDIR)
			{
				LOG(LogLevel::Error) << "path is not a directory: " << path;
			}else{
				LOG(LogLevel::Error) << "unable to open directory: " << path;
			}
		}else{
			struct dirent *dirent_ptr;
			while((dirent_ptr = read_dir(dir)) != NULL){
				string filename(dirent_ptr->name);
				string absolutePath(path+"/"+filename);
				mode_t mode = getFileMode(absolutePath);
				if(S_ISREG(mode))
				{
					filenames.insert(relativePath+"/"+filename);
				}else if(S_ISDIR(mode))
				 {
				 	GetAllFilesInFolderRecursive(absolutePath, relativePath+"/"+filename, filenames);
				 }
			}
			if(errno != 0){
				fileNames.clear();
				LOG(LogLevel::Error) << "an error occurred hile trying to list files in path: " << path;
			}
			closedir(dir);
		}
	}
	
	void GetAllFilesInFolderRecursive(const std::string& path,  std::set<std::string>& filenames)
	{
		GetAllFilesInFolderRecursive(path, "", filenames);
	}

	bool TryCopyFile(const std::string& sourcePath, const std::string& destPath)
	{
		int input_handle = open(sourcePath.c_str(), O_RDONLY);
		if(input_handle == 0){
			LOG(LogLevel::Error) << "unable to open copy source file " << sourcePath;
			return false;
		}
		struct stat input_stat;
		if(fstat(input_handle, &input_stat) != 0){
			close(input_handle);
			LOG(LogLevel::Error) << "unable to determine copy source file size " << sourcePath;
			return false;
		}
		int output_handle = open(destPath.c_str(), O_WRONLY | O_CREAT, input_stat.st_mode);
		if(output_handle == 0){
			LOG(LogLevel::Error) << "unable to open copy destination file " << destPath;
			close(input_handle);
			return false;
		}
		ssize remaining = input_stat.st_size;
		while(remaining > 0)
		{
			ssize copied = sendfile(output_handle, input_handle, remaining);
			if(copied == -1){
				close(input_handle);
				close(output_handle);
				return false;
			}else{
				remaining-=copied;	
			}
		}
		close(input_handle);
		close(output_handle);
		return true;
	}

	bool copyFolder(const std::string& sourceFolder, const std::string& destFolder)
	{
		LOG(LogLevel::Error) << "copyFolder() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
		return false;
	}

	bool renameFolder(const std::string& sourceFolder, const std::string& destFolder)
	{
		LOG(LogLevel::Error) << "renameFolder() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
		return false;
	}

	bool DoesFileExist(const std::string& path)
	{
		LOG(LogLevel::Error) << "DoesFileExist() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}

	bool doesFolderExist(const std::string& path)
	{
		LOG(LogLevel::Error) << "doesFolderExist() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}

	void WriteToConsole(LogLevel level, const std::string& logMessage)
	{
		if(level != LogLevel::Debug) // Don't log debug messages to console.
		{
			std::cout << logMessage;
		}
	}

	std::string GetLastErrorString()
	{
		LOG(LogLevel::Error) << "GetLastErrorString() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
		return "";
	}

	bool deleteFolder(const std::string& folder)
	{
		LOG(LogLevel::Error) << "deleteFolder() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
		return false;
	}

	std::string convertUTF8ToASCII(std::string UTF8)
	{
		LOG(LogLevel::Error) << "convertUTF8ToASCII() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}

	std::string convertUTF8To8859_15(std::string UTF8)
	{
		LOG(LogLevel::Error) << "convertUTF8To8859_15() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}
	
	std::string convertUTF16ToUTF8(std::wstring UTF16)
	{
		LOG(LogLevel::Error) << "convertUTF16ToUTF8() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}

	std::string convert8859_15ToUTF8(std::string input)
	{
		LOG(LogLevel::Error) << "convert8859_15ToUTF8() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}

	std::wstring convert8859_15ToUTF16(std::string UTF8)
	{
		LOG(LogLevel::Error) << "convert8859_15ToUTF16() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}

	std::wstring convertUTF8ToUTF16(std::string UTF8)
	{
		LOG(LogLevel::Error) << "convertUTF8ToUTF16() has been stubbed out in LinuxUtils.cpp.";
		exit(-1);
	}

	int FromMultiByte(const char* in, size_t inSize, wchar_t* out, size_t outSize)
	{
		/*if(outSize == 0)
			return inSize;

		if(inSize <= outSize)
			memcpy(out, in, inSize);

		/*iconv_t conv = iconv_open("CP1252", "UTF-16");
		iconv(conv, &in, &inSize, (char*)&out, &outSize);
		iconv_close(conv);*/
	}

	int ToMultiByte(const wchar_t* in, size_t inSize, char* out, size_t outSize)
	{
		/*if(outSize == 0)
			return inSize;

		if(inSize <= outSize)
			memcpy(out, in, inSize);
		/*iconv_t conv = iconv_open("UTF-16", "CP1252");
		iconv(conv, (char*)&in, &inSize, &out, &outSize);
		iconv_close(conv);*/
	}
}
