/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "CultureGroups.h"
#include "../Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"



class group: commonItems::parser
{
	public:
		group(std::istream& theStream);

		auto getCultures() const { return cultures; }

	private:
		std::vector<std::string> cultures;
};


group::group(std::istream& theStream)
{
	registerKeyword(std::regex("union"), commonItems::ignoreItem);
	registerKeyword(std::regex("leader"), commonItems::ignoreItem);
	registerKeyword(std::regex("unit"), commonItems::ignoreItem);
	registerKeyword(std::regex("is_overseas"), commonItems::ignoreItem);
	registerKeyword(std::regex("[a-zA-Z_]+"), [this](const std::string& cultureName, std::istream& theStream){
		cultures.push_back(cultureName);
		commonItems::ignoreItem(cultureName, theStream);
	});

	parseStream(theStream);
}



Vic2::cultureGroups::cultureGroups() noexcept
{
	registerKeyword(std::regex("[a-zA-Z_]+"), [this](const std::string& groupName, std::istream& theStream){
		group newCultureGroup(theStream);
		for (auto culture: newCultureGroup.getCultures())
		{
			mappings.insert(make_pair(culture, groupName));
		}
	});

	LOG(LogLevel::Info) << "Determining culture groups";

	for (auto mod: theConfiguration.getVic2Mods())
	{
		LOG(LogLevel::Debug) << "Reading mod cultures from " << mod;
		parseFile((theConfiguration.getVic2Path() + "/mod/" + mod + "/common/cultures.txt"));
	}

	parseFile((theConfiguration.getVic2Path() + "/common/cultures.txt"));
}


std::optional<std::string> Vic2::cultureGroups::getGroup(const std::string& culture) const
{
	if (auto mapping = mappings.find(culture); mapping != mappings.end())
	{
		return mapping->second;
	}
	else
	{
		return {};
	}
}