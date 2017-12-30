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



#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"
#include "Object.h"
#include "Log.h"
#include <vector>
using namespace std;



Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	LOG(LogLevel::Info) << "Reading configuration file";

	shared_ptr<Object> configObj = parser_UTF8::doParseFile("configuration.txt");
	if (configObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not open configuration.txt";
		exit(-1);
	}

	vector<shared_ptr<Object>> obj = configObj->getValue("configuration");
	if (obj.size() != 1)
	{
		LOG(LogLevel::Error) << "Configuration file must contain exactly one configuration section";
		exit (-1);
	}

	EU4Path = obj[0]->safeGetString("EU4directory");
	if (!Utils::doesFolderExist(EU4Path))
	{
		LOG(LogLevel::Error) << "No Europa Universalis 4 path was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else if (!Utils::DoesFileExist(EU4Path + "/map/positions.txt"))
	{
		LOG(LogLevel::Error) << EU4Path << " does not appear to be a valid EU4 install";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "EU4 install path is " << EU4Path;
	}


	EU4DocumentsPath	= obj[0]->safeGetString("EU4DocumentsDirectory");
	CK2ExportPath		= obj[0]->safeGetString("CK2ExportDirectory");

	V2Path = obj[0]->safeGetString("V2directory");
	if (Utils::DoesFileExist(V2Path))
	{
		LOG(LogLevel::Error) << "No Victoria 2 path was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "Victoria 2 install path is " << V2Path;
	}

	V2DocumentsPath = obj[0]->safeGetString("V2Documentsdirectory");
	if (Utils::DoesFileExist(V2DocumentsPath))
	{
		LOG(LogLevel::Error) << "No Victoria 2 documents directory was specified in configuration.txt, or the path was invalid";
		exit(-1);
	}
	else
	{
		LOG(LogLevel::Debug) << "Victoria 2 documents directory is " << V2DocumentsPath;
	}

	V2Gametype			= obj[0]->safeGetString("V2gametype");
	resetProvinces		= "no";//obj[0]->safeGetString("resetProvinces");
	MaxLiteracy			= obj[0]->safeGetFloat("max_literacy");
	Removetype			= obj[0]->safeGetString("Removetype");
	libertyThreshold	= obj[0]->safeGetFloat("libertyThreshold");
	convertPopTotals	= obj[0]->safeGetString("convertPopTotals") == "yes";
	outputName			= "";
}