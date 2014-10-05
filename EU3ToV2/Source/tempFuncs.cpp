/*Copyright (c) 2014 The Paradox Game Converters Project

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


/*
+// Directory and File search code
+
+vector<string> fileList;
+
+string folderName = //set the root folder to search here
+
+list<string> foldersToProcess
+foldersToProcess.push_back("");
+while (!foldersToProcess.empty())
+{
+	struct _finddata_t	directoriesData;
+	long						fileListing;
+
+	string currentFolder = foldersToProcess.front();
+	foldersToProcess.pop_front();
+	if ( (fileListing = _findfirst((folderName + currentFolder + "*").c_str(), &directoriesData)) == -1L)
+	{
+		log("\tError while processing directory structure\n");
+		continue;
+	}
+	do
+	{
+		if (strcmp(directoriesData.name, ".") == 0 || strcmp(directoriesData.name, "..") == 0 )
+		{
+			continue;
+		}
+		if (directoriesData.attrib & _A_SUBDIR)
+		{
+			//do whatever processing you need for each directory
+
+			foldersToProcess.push_back(currentFolder + directoriesData.name + "\\");
+		}
+		else
+		{
+			if (TRUE == PathMatchSpec(directoriesData.name, /*a string that matches the files you care about. Can be simple "*."*//*) )
+			{
+				fileList.push_back(currentFolder + directoriesData.name);
+
+				//do whatever processing you need for each file
+			}
+		}
+	} while(_findnext(fileListing, &directoriesData) == 0);
+		_findclose(fileListing);
+}
+*/
