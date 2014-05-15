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

// Returns a formatted string describing the last error on the WinAPI.
std::string GetLastWindowsError();

} // namespace WinUtils

#endif // WINUTILS_H_