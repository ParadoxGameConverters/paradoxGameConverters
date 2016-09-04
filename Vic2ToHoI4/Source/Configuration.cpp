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

	V2Path				= obj[0]->getLeaf("V2directory");
	HoI4Path				= obj[0]->getLeaf("HoI4directory");
	HoI4DocumentsPath = obj[0]->getLeaf("HoI4Documentsdirectory");
	outputName			= "";

	vector<Object*> modsObj = obj[0]->getValue("Vic2Mods");
	if (modsObj.size() > 0)
	{
		Vic2Mods = modsObj[0]->getTokens();
	}

	manpowerFactor			= stof(obj[0]->getLeaf("manpower_factor"));
	icConversion			= obj[0]->getLeaf("ic_conversion");
	icFactor					= stof(obj[0]->getLeaf("ic_factor"));
	if (obj[0]->getValue("ICStats").size() > 0)
	{
		ICStats = true;
	}
	else
	{
		ICStats = false;
	}

	leaderID					= 1000;
	leaderIDCountryIdx	= 1;
}