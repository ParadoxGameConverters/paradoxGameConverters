/*
// Directory and File search code

vector<string> fileList;

string folderName = //set the root folder to search here

list<string> foldersToProcess
foldersToProcess.push_back("");
while (!foldersToProcess.empty())
{
	struct _finddata_t	directoriesData;
	long						fileListing;

	string currentFolder = foldersToProcess.front();
	foldersToProcess.pop_front();
	if ( (fileListing = _findfirst((folderName + currentFolder + "*").c_str(), &directoriesData)) == -1L)
	{
		log("\tError while processing directory structure\n");
		continue;
	}
	do
	{
		if (strcmp(directoriesData.name, ".") == 0 || strcmp(directoriesData.name, "..") == 0 )
		{
			continue;
		}
		if (directoriesData.attrib & _A_SUBDIR)
		{
			//do whatever processing you need for each directory

			foldersToProcess.push_back(currentFolder + directoriesData.name + "\\");
		}
		else
		{
			if (TRUE == PathMatchSpec(directoriesData.name, /*a string that matches the files you care about. Can be simple "*."*//*) )
			{
				fileList.push_back(currentFolder + directoriesData.name);

				//do whatever processing you need for each file
			}
		}
	} while(_findnext(fileListing, &directoriesData) == 0);
		_findclose(fileListing);
}
*/