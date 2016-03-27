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



#ifndef WINUTILS_H_
#define WINUTILS_H_

#include <set>
#include <string>

namespace WinUtils {

// Creates a new folder corresponding to the given path.
// Returns true on success or if the folder already exists.
// Returns false and logs a warning on failure.
bool TryCreateFolder(const std::string& path);
// Adds all files (just the file name) in the specified folder to the given collection.
void GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames);
// Copies the file specified by sourcePath as destPath.
// Returns true on success.
// Returns false and logs a warning on failure.
bool TryCopyFile(const std::string& sourcePath, const std::string& destPath);
// Returns true if the specified file exists (and is a file rather than a folder).
bool DoesFileExist(const std::string& path);
// Returns true if the specified folder exists (and is a folder rather than a file).
bool doesFolderExist(const std::string& path);

// Returns a formatted string describing the last error on the WinAPI.
std::string GetLastWindowsError();

} // namespace WinUtils

#endif // WINUTILS_H_