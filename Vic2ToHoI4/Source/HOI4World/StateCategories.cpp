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



#include "StateCategories.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "../Configuration.h"
#include <set>



class stateCategory: commonItems::parser
{
	public:
		explicit stateCategory(std::istream& theStream);

		auto getNumberOfSlots() const { return numberOfSlots; }

	private:
		int numberOfSlots = 0;
};


stateCategory::stateCategory(std::istream& theStream)
{
	registerKeyword(std::regex("local_building_slots"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt slotsInt(theStream);
		numberOfSlots = slotsInt.getInt();
	});
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}


class stateCategoryFile: commonItems::parser
{
	public:
		explicit stateCategoryFile(std::istream& theStream);

		auto getCategories() const { return theCategories; }

	private:
		std::map<int, std::string> theCategories;
};


stateCategoryFile::stateCategoryFile(std::istream& theStream)
{
	registerKeyword(std::regex("[a-z\\_]+"), [this](const std::string& categoryName, std::istream& theStream){
		stateCategory category(theStream);
		theCategories.insert(make_pair(category.getNumberOfSlots(), categoryName));
	});

	parseStream(theStream);
}


HoI4::stateCategories::stateCategories() noexcept
{
	registerKeyword(std::regex("state_categories"), [this](const std::string& unused, std::istream& theStream){
		stateCategoryFile theFile(theStream);
		for (auto category: theFile.getCategories())
		{
			theCategories.insert(category);
		}
	});

	std::set<std::string> categoryFiles;
	Utils::GetAllFilesInFolder(theConfiguration.getHoI4Path() + "/common/state_category", categoryFiles);
	for (auto file: categoryFiles)
	{
		parseFile(theConfiguration.getHoI4Path() + "/common/state_category/" + file);
	}
}


std::string HoI4::stateCategories::getBestCategory(int numBuildingSlots) const
{
	std::string theCategory;
	for (auto possibleCategory: theCategories)
	{
		if (numBuildingSlots >= possibleCategory.first)
		{
			theCategory = possibleCategory.second;
		}
	}

	return theCategory;
}