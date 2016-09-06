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



#include "Configuration.h"
#include "ParadoxParserUTF8.h"
#include "Object.h"
#include "Log.h"
#include <vector>
using namespace std;



Configuration* Configuration::instance = NULL;

Configuration::Configuration()
{
	LOG(LogLevel::Info) << "Reading configuration file";

	Object* oneObj = parser_UTF8::doParseFile("configuration.txt");	// the parsed configuration file
	if (oneObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not open configuration.txt";
		exit(-1);
	}

	vector<Object*> obj = oneObj->getValue("configuration");	// the configuration section
	if (obj.size() != 1)
	{
		LOG(LogLevel::Error) << "Configuration file must contain exactly one configuration section";
		exit (-1);
	}

	EU4Path				= obj[0]->getLeaf("EU4directory");
	EU4DocumentsPath	= obj[0]->getLeaf("EU4DocumentsDirectory");
	CK2ExportPath		= obj[0]->getLeaf("CK2ExportDirectory");
	V2Path				= obj[0]->getLeaf("V2directory");
	V2DocumentsPath	= obj[0]->getLeaf("V2Documentsdirectory");
	V2Gametype			= obj[0]->getLeaf("V2gametype");
	resetProvinces		= obj[0]->getLeaf("resetProvinces");
	MaxLiteracy			= atof(obj[0]->getLeaf("max_literacy").c_str());
	Removetype			= obj[0]->getLeaf("Removetype");
	libertyThreshold	= atof(obj[0]->getLeaf("libertyThreshold").c_str());
	convertPopTotals	= (obj[0]->getLeaf("convertPopTotals") == "yes");
	outputName			= "";
}