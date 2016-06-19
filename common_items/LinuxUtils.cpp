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

#include "OSCompatabilityLayer.h"

#include <iostream>
#include <stdarg.h>
#include <stdio.h>

#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>

#include <iconv.h>

static boost::system::error_code lastError;

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
  bool TryCreateFolder(const std::string& path)
  {
      return doesFolderExist(path) || boost::filesystem::create_directory(path, lastError);
  }
  
  void getCurrentDirectory(uint32_t length, char directory[MAX_PATH])
  {
    boost::filesystem::current_path(directory, lastError);
  }
  
  bool TryCopyFile(const std::string& sourcePath, const std::string& destPath)
  {
    boost::system::error_code prevError = lastError;
    boost::filesystem::copy_file(sourcePath, destPath, lastError);
    return prevError == lastError;
  }
  
  bool DoesFileExist(const std::string& path)
  {
    return boost::filesystem::exists(path, lastError) && boost::filesystem::is_regular_file(path, lastError);
  }
  
  bool doesFolderExist(const std::string& path)
  {
    return boost::filesystem::exists(path, lastError) && boost::filesystem::is_directory(path, lastError);
  }
  
  int FromMultiByte(const char* in, size_t inSize, wchar_t* out, size_t outSize)
  {
    if(outSize == 0)
      return inSize;
    
    if(inSize <= outSize)
      memcpy(out, in, inSize);
    
    /*iconv_t conv = iconv_open("CP1252", "UTF-16");
    iconv(conv, &in, &inSize, (char*)&out, &outSize);
    iconv_close(conv);*/
  }
  
  int ToMultiByte(const wchar_t* in, size_t inSize, char* out, size_t outSize)
  {
    if(outSize == 0)
      return inSize;
    
    if(inSize <= outSize)
      memcpy(out, in, inSize);
    /*iconv_t conv = iconv_open("UTF-16", "CP1252");
    iconv(conv, (char*)&in, &inSize, &out, &outSize);
    iconv_close(conv);*/
  }
  
  void WriteToConsole(LogLevel level, const std::string& logMessage)
  {
    if(level != LogLevel::Debug) // Don't log debug messages to console.
    {
      std::cout << logMessage;
    }
  }
  
  std::string GetLastError()
  {
    return lastError.message();
  }
}