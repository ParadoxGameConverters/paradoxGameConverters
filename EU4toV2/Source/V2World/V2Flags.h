#ifndef V2FLAGS_H
#define V2FLAGS_H

#include <map>
#include <set>
#include <string>
#include <vector>

class V2Country;

// Holds information about all the flags for V2 countries and copies over
// the appropriate flags with Output().
class V2Flags 
{
public:
	// Tries to find appropriate flags for all the countries specified.
	void SetV2Tags(const std::map<std::string, V2Country*>& V2Countries);
	// Copies all necessary flags to the output folder. Returns true if successful.
	bool Output() const;

private:
	// Adds all files (just the file name) in the specified folder to the given collection.
	static void GetAllFilesInFolder(const std::string& path, std::set<std::string>& fileNames);
	// Creates a new folder corresponding to the given path.
	// Returns true if successful (including the case where the folder already exists).
	// Logs a message and returns false if unsuccessful.
	static bool TryCreateFolder(const std::string& path);
	// Copies the file specified by sourcePath as destPath. Returns true if successful.
	// Logs a message and returns false if unsuccessful.
	static bool TryCopyFile(const std::string& sourcePath, const std::string& destPath);
	// Returns true if the specified file exists (and is a file rather than a folder).
	static bool DoesFileExist(const std::string& path);
	// Returns the text of the last error recorded by the WinAPI.
	static std::string GetLastWindowsError();

	static const std::vector<std::string> flagFileSuffixes;

	typedef std::map<std::string, std::string> V2TagToFlagTagMap;
	V2TagToFlagTagMap tagMapping;
};

#endif