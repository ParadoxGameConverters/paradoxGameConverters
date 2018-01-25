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
#include <algorithm>
#include <set>

#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <iconv.h>
#include <stdlib.h>

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

	/*
		Helper function to determine that returns false if the filename is a:
		- a hidden folder or folder (starting with '.')
		- empty
		- a parent directory ('.')
		- the current directory ('..')
	*/	
	bool IsRegularNodeName(const std::string &name)
	{
	        return !name.empty() && *name.begin() != '.';
	}


	/*
        	returns false if a node name is empty or a placeholder for the current or parent directory, true otherwise
	*/
	bool IsActualNodeName(const std::string &name)
	{
        	using namespace std;
		using namespace std;
	        for(string::const_iterator i = name.begin(); i != name.end(); ++i)
		{
        	        if(*i != '.')
			{
                	        return true;
			}
                }
        	return false;
	}


	const char * StripTrailingSeparators(const char *path, std::size_t length){
        	switch(length)
		{
                	case 0:
                        	return path;
                	case 1:
                        	if(*path != '/')
				{
                                	++path;
                        	}
                        	return path;
               		default:
                        	const char *i = path+length-1;
                        	while(i != path && *i == '/')
				{
                                	--i;
                        	}
                        	++i;
                        	return i;
        	}
	}

	const char *StripLeadingSeparators(const char *path)
	{
        	while(true)
		{
                	char c = *path;
                	if(c != '/')
			{
                        	return path;
                	}
                	++path;
        	}
	}


	/*
		Concatenates two paths and strips out leading and trailing '/'
	*/
	std::string ConcatenatePaths(const std::string &first, const std::string &second)
	{
        	using namespace std;
        	const char *first_begin = first.c_str();
       		const char *first_end = StripTrailingSeparators(first_begin, first.length());
        	const char *second_begin = StripLeadingSeparators(second.c_str());
        	return string(first_begin, first_end) + "/" + string(second_begin);
	}

	/*
		Contatenates a path and a valid node name
		A valid node name is a non empty string that does not contain '/'
		use ConcatenatePaths for other concatenations
	*/
	std::string ConcatenateNodeName(const std::string &path, const std::string &name){
        	using namespace std;
        	const char *path_begin = path.c_str();
        	const char *path_end = StripTrailingSeparators(path_begin, path.length());
        	return string(path_begin, path_end) + "/" + name;
	}
	
	/*
		Splits the node name from the rest of the path and returns both values as a pair (path, nodename)
	*/
	std::pair<std::string, std::string> SplitNodeNameFromPath(const std::string &path){
        	using namespace std;
        	if(path.empty())
		{
                	return make_pair(string(), string());
        	}
        	const char *buffer = path.c_str();
        	const char *end = buffer+path.length()-1;
        	while(end != buffer && *end == '/')
		{
                	--end;
        	}
        	if(end == buffer)
		{
                	return make_pair(string(buffer, buffer+1), string());
        	}
        	const char *begin = end;
        	++end;
        	while(begin != buffer && *begin != '/')
		{
                	--begin;
        	}
        	if(*begin == '/')
		{
                	++begin;
       		}

        	return make_pair(string(buffer,begin),string(begin, end));
	}

	bool IsLinuxPathElementSeparator(char c){
		return c == '/';
	}
	
	bool isLinuxPathCharacter(char c){
		return c != '/';
	}


	std::string resolvePath(const std::string &path){
		using namespace std;
		if(path.empty() || path.length() > PATH_MAX){
			return path;
		}
		char buffer[PATH_MAX];
		if(realpath(path.c_str(), buffer) == NULL){
			return path;
		}
		return string(buffer);
	};

	std::string resolveParentPath(const std::string &path){
		using namespace std;
		pair<string, string> split = SplitNodeNameFromPath(path);
		if(split.first.empty()){
			return path;
		}else{
			return ConcatenatePaths(resolvePath(split.first), split.second);
		}
	};

	bool TryCreateFolderNonRecursive(const char *unresolvedPath)
	{
        	using namespace std;
		const string path = resolveParentPath(string(unresolvedPath));
		const mode_t mode = S_IRWXU | S_IRWXG | S_IROTH;
        	struct stat status;
        	if(stat(path.c_str(), &status) != 0)
			{
                	if(mkdir(path.c_str(), mode) != 0 && errno != EEXIST)
			{
                        	LOG(LogLevel::Error) << "unable to create folder: " << path;
                       		return false;
                	}
        	}else{
                	if(!S_ISDIR(status.st_mode))
			{
                        	LOG(LogLevel::Error) << "node already exists but is not a folder: " << path;
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
	
	bool GetFileMode(const std::string &path, mode_t &result)
	{
		using namespace std;
                struct stat status;
                if(stat(path.c_str(), &status) != 0)
                {
                        return false;
                }else{
                        result = status.st_mode;
                        return true;
                }
	}

	bool IsRegularFile(const std::string &path)
	{
                mode_t mode;
                return GetFileMode(path, mode) && S_ISREG(mode);
	}	
	
	/*
		Note: since the function signature did not allow for a return value, it clears the fileNames set when an error occurs to make sure no operations are done on an incomplete list of files
	*/
	void GetAllFilesInFolder(const std::string& unresolvedPath, std::set<std::string>& fileNames)
	{
                using namespace std;
		const string path = resolvePath(unresolvedPath);
                DIR *dir = opendir(path.c_str());
                if(dir == NULL)
                {
			LOG(LogLevel::Error) << "unable to get all files in folder: " << path;
                        if(errno == EACCES)
                        {
                                LOG(LogLevel::Error) << "\tno permission to read directory ";
                        }else if(errno == ENOENT)
                        {
                                LOG(LogLevel::Error) << "\tdirectory does not exist";
                        }else if(errno == ENOTDIR)
                        {
                                LOG(LogLevel::Error) << "\tpath is not a directory";
                        }else{
                                LOG(LogLevel::Error) << "\tunable to open directory";
                        }
                }else{
                        struct dirent *dirent_ptr;
                        while((dirent_ptr = readdir(dir)) != NULL){
                                string filename{dirent_ptr->d_name};
                                if(IsRegularNodeName(filename) && IsRegularFile(ConcatenateNodeName(path,filename))){
                                        if(errno != 0){
                                                fileNames.clear();
                                                closedir(dir);
                                                LOG(LogLevel::Error) << "an error occurred hile trying to list files in path: " << path;
                                                return;
                                        }
                                        fileNames.insert(filename);
                                }
                        }
                        closedir(dir);
                }
        }

	void GetAllFilesInFolderRecursiveWithRelativePath(const std::string& path, const std::string &relative_path, std::set<std::string>& filenames)
        {
                using namespace std;
                DIR *dir = opendir(path.c_str());
                if(dir == NULL)
                {
			LOG(LogLevel::Error) << "unable to get all files in folder (recursive): " << path;
                        if(errno == EACCES)
                        {
                                LOG(LogLevel::Error) << "\tno permission to read directory";
                        }else if(errno == ENOENT)
                        {
                                LOG(LogLevel::Error) << "\tdirectory does not exist";
                        }else if(errno == ENOTDIR)
                        {
                                LOG(LogLevel::Error) << "\tpath is not a directory";
                        }else{
                                LOG(LogLevel::Error) << "\tunable to open directory";
                        }
                }else{
                        struct dirent *dirent_ptr;
                        while((dirent_ptr = readdir(dir)) != NULL){
                                string filename(dirent_ptr->d_name);
                                if(IsRegularNodeName(filename)){
                                        string absolute_path(ConcatenateNodeName(path,filename));
                                        mode_t mode;
                                        if(!GetFileMode(absolute_path, mode)){
                                                filenames.clear();
                                                closedir(dir);
                                                LOG(LogLevel::Error) << "unable to check mode for path " << absolute_path;
                                                return;
                                        }
                                        if(S_ISREG(mode))
                                        {
                                                filenames.insert(ConcatenateNodeName(relative_path,filename));
                                        }else if(S_ISDIR(mode)){
                                                GetAllFilesInFolderRecursiveWithRelativePath(absolute_path, ConcatenateNodeName(relative_path,filename), filenames);
                                        }
                                }
                        }
                        closedir(dir);
                }
	}

	/*
                Note: since the function signature did not allow for a return value, it clears the fileNames set when an error occurs to make sure no operations are done on an incomplete list of files
        */
	void GetAllFilesInFolderRecursive(const std::string& unresolvedPath, std::set<std::string>& filenames)
	{
		using namespace std;
		const string path = resolvePath(unresolvedPath);
                GetAllFilesInFolderRecursiveWithRelativePath(path, "", filenames);
	}

	bool TryCopyFile(const std::string& unresolvedSourcePath, const std::string& unresolvedDestPath)
        {
                using namespace std;
		const string sourcePath = resolvePath(unresolvedSourcePath);
		const string destPath = resolveParentPath(unresolvedDestPath);
                int inputHandle = open(sourcePath.c_str(), O_RDONLY);
                if(inputHandle == -1)
                {
                        LOG(LogLevel::Error) << "unable to open copy source path: " << sourcePath;
                        return false;
                }
                struct stat inputStat;
                if(fstat(inputHandle, &inputStat) != 0){
                        close(inputHandle);
                        LOG(LogLevel::Error) << "unable to determine copy source file size: " << sourcePath;
                        return false;
                }
                if(!S_ISREG(inputStat.st_mode)){
                        close(inputHandle);
                        LOG(LogLevel::Error) << "copy source path is a directory: " << sourcePath;
                        return false;
                }
                int outputHandle = open(destPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, inputStat.st_mode);
                if(outputHandle == -1)
                {
                        LOG(LogLevel::Error) << "unable to open copy destination file: " << destPath;
                        close(inputHandle);
                        return false;
                }
                ssize_t remaining = inputStat.st_size;
                while(remaining > 0)
                {
        		// Copy file using sendfile because this is more efficient since copy is done in kernelspace
			// Won't work in kernels older than 2.6.33 (glibc 2.1)
			// If such systems must be supported, this method should be reimplemented using POSIX read/write methods or iostreams
	               ssize_t copied = sendfile(outputHandle, inputHandle, NULL, remaining);
                        if(copied == -1){
                                close(inputHandle);
                                close(outputHandle);
                                return false;
                        }else{
                                remaining-=copied;
                        }
                }
                close(inputHandle);
                close(outputHandle);
                return true;
	}

	bool CopyFolderAndFiles(const std::string& unresolvedSourceFolder, const std::string& unresolvedDestParentFolder, const std::string &folderName)
	{
                using namespace std;
		const string sourceFolder = resolvePath(unresolvedSourceFolder);
		const string destParentFolder = resolvePath(unresolvedDestParentFolder);
                DIR *dir = opendir(sourceFolder.c_str());
                if(dir == NULL)
                {
			LOG(LogLevel::Error) << "unable to copy folder and files from source folder: " << sourceFolder;
                        if(errno == EACCES)
                        {
                                LOG(LogLevel::Error) << "\tno permission to read directory";
                        }else if(errno == ENOENT)
                        {
                                LOG(LogLevel::Error) << "\tdirectory does not exist";
                        }else if(errno == ENOTDIR)
                        {
                                LOG(LogLevel::Error) << "\tpath is not a directory";
                        }else{
                                LOG(LogLevel::Error) << "\tunable to open directory";
                        }
                        return false;
                }else{
                        string destFolder(ConcatenateNodeName(destParentFolder, folderName));
                        if(!TryCreateFolder(destFolder))
			{
                                closedir(dir);
                                return false;
                        }
                        struct dirent *dirent_ptr;
                        while((dirent_ptr = readdir(dir)) != NULL)
			{
                                string filename(dirent_ptr->d_name);
                                if(IsRegularNodeName(filename))
                                        {
                                        string childSourcePath(ConcatenateNodeName(sourceFolder,filename));
                                        mode_t mode;
                                        if(!GetFileMode(childSourcePath, mode))
                                        {
                                                closedir(dir);
                                                LOG(LogLevel::Error) << "unable to check mode for path " << childSourcePath;
                                                return false;
                                        }
                                        if(S_ISREG(mode))
                                        {
                                                if(!TryCopyFile(childSourcePath, ConcatenateNodeName(destFolder, filename)))
                                                {
                                                        closedir(dir);
                                                        return false;
                                                }
                                        }else if(S_ISDIR(mode))
					{
                                                if(!CopyFolderAndFiles(childSourcePath, destFolder, filename))
						{
                                                        closedir(dir);
                                                        return false;
                                                }
                                        }
                                }
                        }
                        closedir(dir);
                        return true;
                }
	}

	/*
        Warning: This method does not do recursive checking
        copying a folder inside it's own decendant tree may result in a infinite loop until max path length is reached
	*/
	bool copyFolder(const std::string& sourceFolder, const std::string& destFolder)
        {
                using namespace std;
                pair<string, string> pathAndName = SplitNodeNameFromPath(destFolder);
                return CopyFolderAndFiles(sourceFolder, pathAndName.first, pathAndName.second);
        }

	bool renameFolder(const std::string& unresolvedSourceFolder, const std::string& unresolvedDestFolder)
	{
		using namespace std;
		const string sourceFolder = resolvePath(unresolvedSourceFolder);
		const string destFolder = resolvePath(unresolvedDestFolder);
        	if(rename(sourceFolder.c_str(), destFolder.c_str()) != 0)
		{
                	LOG(LogLevel::Error) << "unable to rename folder " << sourceFolder << " to " << destFolder;
                	switch(errno)
			{
                	        case EACCES:
                        	case EPERM:
                        	        LOG(LogLevel::Error) << "\tno permission to move folder";
                        	        break;
                       		case ENOENT:
                                	LOG(LogLevel::Error) << "\tsource folder does not exist";
                                	break;
                        	case EBUSY:
                                	LOG(LogLevel::Error) << "\tsource or destination folder is locked by another process";
                                	break;
                        	case EEXIST:
                        	case ENOTEMPTY:
                                	LOG(LogLevel::Error) << "\tdestination folder already exists and is not empty";
                                	break;
                        	case EINVAL:
                                	LOG(LogLevel::Error) << "\tdestination folder contains source folder";
                                	break;
                        	case EISDIR:
                                	LOG(LogLevel::Error) << "\tdestination folder is not a directory";
                                	break;
                	}
        	        return false;
        	}else{
        	        return true;
	        }
	}

	bool DoesFileExist(const std::string& unresolvedPath)
	{
		using namespace std;
		const string path = resolvePath(unresolvedPath);
		mode_t mode;
		return GetFileMode(path, mode) && S_ISREG(mode);
	}

	bool doesFolderExist(const std::string& unresolvedPath)
	{
		using namespace std;
		const string path = resolvePath(unresolvedPath);
		mode_t mode;
		return GetFileMode(path, mode) && S_ISDIR(mode);
	}

	void WriteToConsole(LogLevel level, const std::string& logMessage)
	{
		if(level != LogLevel::Debug) // Don't log debug messages to console.
		{
			std::cout << logMessage;
		}
	}

	/*
		Implemented all messages for error codes used in LinuxUtils.cpp.
	*/
	std::string GetLastErrorString()
	{
		using namespace std;
		switch(errno)
		{
			case EEXIST:
				return string("Error code: EEXIST");
			case EACCES:
				return string("Error code: EACCES");
			case ENOENT:
				return string("Error code: ENOENT");
			case ENOTDIR:
				return string("Error code: ENOTDIR");
			case EPERM:
				return string("Error code: EPERM");
			case EBUSY:
				return string("Error code: EBUSY");
			case ENOTEMPTY:
				return string("Error code: ENOTEMPTY");
			case EINVAL:
				return string("Error code: EINVAL");
			case EISDIR:
				return string("Error code: EISDIR");
			case EROFS:
				return string("Error code: EROFS");	
			case E2BIG:
				return string("Error code: E2BIG");
			case EILSEQ:
				return string("Error code: EILSEQ");
			default:
				return string("Error code: unknown");
		}
	}

	bool DeleteFile(const std::string &unresolvedFile)
	{
		using namespace std;
		const string file = resolvePath(unresolvedFile);
		if(unlink(file.c_str()) != 0)
		{
			LOG(LogLevel::Error) << "unable to delete file " << file;
			switch(errno)
			{
				case ENOENT:
				case ENOTDIR:
					LOG(LogLevel::Error) << "\tpath does not point to a valid file";
					break;
				case EPERM:
				case EACCES:
					LOG(LogLevel::Error) << "\tyou do not have permission to delete the file";
					break;
				case EBUSY:
					LOG(LogLevel::Error) << "\tanother process has opened the file";
					break;
				case EROFS:
					LOG(LogLevel::Error) << "\tthe filesystem is mounted with the read only flag";
					break;
			}
			return false;
		}else{
			return true;
		}
	}

	bool DeleteEmptyFolder(const std::string &unresolvedFolder){
		using namespace std;
		const string folder = resolvePath(unresolvedFolder);
		if(rmdir(folder.c_str()) != 0)
		{
			LOG(LogLevel::Error) << "unable to delete folder " << folder;
			switch(errno)
			{
				case ENOTEMPTY:
				case EEXIST:
					LOG(LogLevel::Error) << "\tfolder is not empty";
					break;
				case ENOENT:
				case ENOTDIR:
					LOG(LogLevel::Error) << "\tpath does not point to a valid folder";
					break;
				case EPERM:
				case EACCES:
					LOG(LogLevel::Error) << "\tyou do not have permission to delete the file";
					break;
				case EBUSY:
					LOG(LogLevel::Error) << "\tanother process has opened this folder ";
					break;
				case EROFS:
					LOG(LogLevel::Error) << "\tthe filesystem is mounted with the read only flag";
					break;
			}
			return false;
		}else{
			return true;
		}

	}

	bool deleteFolder(const std::string& unresolvedFolder)
	{
		using namespace std;
         	const string folder = resolvePath(unresolvedFolder);
	        DIR *dir = opendir(folder.c_str());
                if(dir == NULL)
                {
                        LOG(LogLevel::Error) << "unable to read folder prior to delete " << folder;
                        if(errno == EACCES)
                        {
                                LOG(LogLevel::Error) << "\tno permission to read directory";
                        }else if(errno == ENOENT)
                        {
                                LOG(LogLevel::Error) << "\tdirectory does not exist";
                        }else if(errno == ENOTDIR)
                        {
                                LOG(LogLevel::Error) << "\tpath is not a directory";
                        }else{
                                LOG(LogLevel::Error) << "\tunable to open directory";
                        }
                        return false;
                }else{
                        struct dirent *dirent_ptr;
                        while((dirent_ptr = readdir(dir)) != NULL){
                                string filename(dirent_ptr->d_name);
                                if(IsActualNodeName(filename))
                                {
                                        std::string childPath = ConcatenateNodeName(folder, filename);
                                        mode_t mode;
                                        if(!GetFileMode(childPath, mode))
                                        {
                                                closedir(dir);
                                                LOG(LogLevel::Error) << "unable to check mode for path " << childPath;
                                                return false;
                                        }
                                        if(S_ISDIR(mode)){
                                                if(!deleteFolder(childPath)){
                                                        closedir(dir);
                                                        return false;
                                                }
                                        }else{
                                                if(!DeleteFile(childPath)){
                                                        closedir(dir);
                                                        return false;
                                                }
                                        }
                                }
                        }
                        closedir(dir);
                        return DeleteEmptyFolder(folder);
               }
	}

	/*
	* Forward declarations for conversion helper function and types
	*/
	class ConversionInputBuffer;

	class ConversionOutputBuffer;

	bool ConvertString(const char *toCode, const char *fromCode, ConversionInputBuffer &in, ConversionOutputBuffer &out);

	/*	
	* helper class to copy buffer from string and keep track of remainder and current position
	*/
	class ConversionInputBuffer
	{
        	char *data;
		char *in_buffer;
		std::size_t remainder;
	public:
		explicit ConversionInputBuffer(const std::string &input) : data(new char[input.length()]), in_buffer(data), remainder(input.length())
		{
			using namespace std;
			//POSIX iconv expects a pointer to char *, not to const char * and consequently does not guarantee that the input sequence is not modified so we copy it into the buffer before attempting conversion
			copy(input.begin(), input.end(), data);
		};
		
		template<typename String> explicit ConversionInputBuffer(const String &input){
			typedef typename String::value_type Char;
			using namespace std;
			remainder = sizeof(Char) * input.length();
			data = new char[remainder];
			in_buffer = data;
			const char *input_str = reinterpret_cast<const char *>(input.c_str());
			copy(input_str, input_str + remainder, data);
		};

		~ConversionInputBuffer()
		{
			delete[] data;
		};

		bool has_remaining_bytes() const
		{
			return remainder != 0;
		};

		friend bool ConvertBuffer(const char *, const char *, ConversionInputBuffer &, ConversionOutputBuffer &); 
	private:
		//declared private to avoid copy of buffer
		//unsure if versions pre c++11  should be supported, if not, copy constructors can be explicitly deleted
		ConversionInputBuffer(const ConversionInputBuffer &); 
		ConversionInputBuffer &operator=(const ConversionInputBuffer &); 

	};

	/*
	* helper class to manage buffer pointers and sizes while providing access to internal buffers for use in iconv functions
	*/
	class ConversionOutputBuffer
	{   
		std::size_t size;
		std::size_t block_size;
		std::size_t remainder;
		char *data = nullptr;
		char *out_buffer = nullptr;

		template<typename String> struct OutputStrHelper{
			
			typedef typename String::value_type Char;
			
			static void str(String &output, char *buffer, std::size_t length){
				using namespace std;
				const Char *output_buffer = reinterpret_cast<const Char *>(buffer);
				size_t output_length = length / sizeof(Char);
				output.assign(output_buffer,output_length);
			};
		
		};
		
		template<typename Traits, typename Alloc> struct OutputStrHelper<std::basic_string<char, Traits, Alloc> >{
			
			static void str(std::basic_string<char, Traits, Alloc> &output, char *buffer, std::size_t length){
				using namespace std;
				output.assign(buffer, length);
			};
		};
		
	public:
		explicit ConversionOutputBuffer(std::size_t initial_size = 0, std::size_t increment_block_size = 1024*1024) : size(initial_size), remainder(initial_size), block_size(increment_block_size)
		{
			if(size != 0)
			{ 
				data = new char[size];
				out_buffer = data;
			}
		};

		~ConversionOutputBuffer()
		{
			delete[] data;
		};

		bool full() const
		{
			return remainder != 0;
		};

		size_t length() const
		{
			return size-remainder;
		};

		std::string str() const
		{
			using namespace std;
			return string(data, data+length());
		};

		template<typename String> void str(String &output) const{
			OutputStrHelper<String>::str(output, data, length());
		};

		bool ensure_capacity(std::size_t capacity)
		{
			using namespace std;
			if(size < capacity)
			{
				size_t len = length();
				char *ndata = new char[capacity];
				copy(data, data+len, ndata);
				delete[] data;
				data = ndata;
				out_buffer = data+len;
				remainder+=(capacity-size);
				size = capacity;
				return true;
			}else{
				return false;
			}
		};

		bool grow()
		{
			return ensure_capacity(size+block_size);
		};

		friend bool ConvertBuffer(const char *, const char *, ConversionInputBuffer &, ConversionOutputBuffer &);
	private:
		//declared private to avoid copy of buffer
		//unsure if versions pre c++11  should be supported, if not, copy constructors can be explicitly deleted
		ConversionOutputBuffer(const ConversionOutputBuffer &);
		ConversionOutputBuffer &operator=(const ConversionOutputBuffer &);
	};

	bool ConvertBuffer(const char *fromCode, const char *toCode, ConversionInputBuffer &from, ConversionOutputBuffer &to)
	{
		using namespace std;
		iconv_t descriptor = iconv_open(toCode, fromCode);
		if(descriptor == ((iconv_t)(-1)))
		{
			LOG(LogLevel::Error) << "unable to recode string from '" << fromCode << "' to '" << toCode << ": not supported on this system";
			return false;
		}
		while(from.has_remaining_bytes())
		{
			if(iconv(descriptor, &from.in_buffer, &from.remainder, &to.out_buffer, &to.remainder) == ((size_t)(-1)))
			{
				switch(errno)
				{
					case E2BIG:
						to.grow();
						break;
					case EILSEQ:
						LOG(LogLevel::Error) << "invalid input sequence encountered during conversion from " << fromCode << " to " << toCode;
						return false;
					case EINVAL:
						LOG(LogLevel::Error) << "incomplete input sequence encountered during conversion from " << fromCode << " to " << toCode;
						return false;
				}
			}
		}
		iconv_close(descriptor);
		return true;
	}

	/*
	* InputString and OutputString should be instantiations of basic_string, or have a similar API
	* InputString should implement:
	* member type value_type
	* member function value_type begin() const
	* member function value_type end() const
	* member function size_t length() const
	*
	*/
	template<typename InputString, typename OutputString> bool ConvertString(const char *fromCode, const char *toCode, const InputString &from, OutputString &to, std::size_t to_buffer_size = 0)
	{
		using namespace std;
		if(to_buffer_size == 0)
		{
			to_buffer_size = from.length();
		}
		ConversionInputBuffer from_buffer(from);
		ConversionOutputBuffer to_buffer(to_buffer_size);
		if(ConvertBuffer(fromCode, toCode, from_buffer, to_buffer))
		{
			to_buffer.str(to);
			return true;
		}else{
			return false;
		}
	}
	
	template<typename InputString, typename OutputString> OutputString ConvertString(const char *fromCode, const char *toCode, const InputString &from, std::size_t to_buffer_size = 0)
	{
		using namespace std;
		OutputString to;
		ConvertString(fromCode, toCode, from, to, to_buffer_size);
		return to;
	}

	std::string convertUTF8ToASCII(const std::string& UTF8)
	{
	        using namespace std;
		return ConvertString<string, string>("UTF-8", "ASCII", UTF8);
	}

	std::string convertUTF8To8859_15(const std::string& UTF8)
	{
		using namespace std;
		return ConvertString<string, string>("UTF−8", "ISO−8859−15", UTF8);
	}
	
	/*
		Warning: The input string should not be encoded in UTF-16 but in the system dependent wchar_t encoding
		see convertUTF8ToUTF16 for full explanation
	*/
	std::string convertUTF16ToUTF8(std::wstring UTF16)
	{
		using namespace std;
		return ConvertString<wstring, string>("wchar_t", "UTF-8", UTF16);
	}

	std::string convert8859_15ToUTF8(const std::string& input)
	{
		using namespace std;
		return ConvertString<string, string>("ISO−8859−15", "UTF-8", input);
	}

	/*
		Warning: Does not actually return an UTF-16 sequence.
		This is an implementation of the original Windows-based API which uses UTF-16 LE as the system dependent wchar_t encoding
		This behaviour is replicated on Linux but it uses the (system dependent) wchar_t encoding.
	*/
	std::wstring convert8859_15ToUTF16(std::string UTF8)
	{
		using namespace std;
		return ConvertString<string, wstring>("ISO−8859−15", "wchar_t", UTF8);
	}

	/*
		Warning: Does not actually return an UTF-16 sequence.
		This is an implementation of the original Windows-based API which uses UTF-16 LE as the system dependent wchar_t encoding
		This behaviour is replicated on Linux but it uses the (system dependent) wchar_t encoding.
	*/
	std::wstring convertUTF8ToUTF16(std::string UTF8)
	{
		using namespace std;
		return ConvertString<string, wstring>("UTF-8", "wchar_t",UTF8);
	}

	
	std::string convertToUTF8(const std::wstring &input)
	{
		using namespace std;
		return ConvertString<wstring, string>("wchar_t", "UTF-8",input);
	}

	std::string normalizeUTF8Path(const std::string &utf_8_path){
		return utf_8_path;
	};
}
