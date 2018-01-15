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



#include "ImpassableProvinceMapper.h"
#include "../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParadoxParserUTF8.h"



impassableProvinceMapper* impassableProvinceMapper::instance = nullptr;



impassableProvinceMapper::impassableProvinceMapper()
{
	LOG(LogLevel::Info) << "Finding impassable provinces";

	set<string> statesFiles;
	Utils::GetAllFilesInFolder(Configuration::getHoI4Path() + "/history/states", statesFiles);
	for (auto stateFile: statesFiles)
	{
		int num = stoi(stateFile.substr(0, stateFile.find_first_of('-')));

		auto fileObj = parser_UTF8::doParseFile(Configuration::getHoI4Path() + "/history/states/" + stateFile);
		if (fileObj)
		{
			auto stateObj = fileObj->safeGetObject("state");
			auto impassable = stateObj->safeGetString("impassable", "no");
			if (impassable == "yes")
			{
				auto provincesObj = stateObj->safeGetObject("provinces");
				auto tokens = provincesObj->getTokens();

				for (auto provinceNumString: tokens)
				{
					impassibleProvinces.insert(stoi(provinceNumString));
				}
			}
		}
		else
		{
			LOG(LogLevel::Error) << "Could not parse " << Configuration::getHoI4Path() << "/history/states/" << stateFile;
			exit(-1);
		}
	}
}


bool impassableProvinceMapper::IsProvinceImpassable(int provinceNumber) const
{
	return (impassibleProvinces.count(provinceNumber) > 0);
}