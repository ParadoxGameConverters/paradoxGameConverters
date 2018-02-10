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



#include "ColonialRegions.h"
#include "../Configuration.h"
#include "Color.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <algorithm>



namespace EU4
{
	class colonialRegion: commonItems::parser
	{
		public:
			colonialRegion(std::istream& theStream);

			std::set<int> getProvinces() const { return provinces; }

		private:
			std::set<int> provinces;
	};


	colonialRegion::colonialRegion(std::istream& theStream)
	{
		registerKeyword(std::regex("color"), [this](const std::string& unused, std::istream& theStream)
		{
			commonItems::Color unusedColor(theStream);
		});
		registerKeyword(std::regex("tax_income"), commonItems::ignoreObject);
		registerKeyword(std::regex("native_size"), commonItems::ignoreObject);
		registerKeyword(std::regex("native_ferocity"), commonItems::ignoreObject);
		registerKeyword(std::regex("native_hostileness"), commonItems::ignoreObject);
		registerKeyword(std::regex("trade_goods"), commonItems::ignoreObject);
		registerKeyword(std::regex("culture"), commonItems::ignoreObject);
		registerKeyword(std::regex("religion"), commonItems::ignoreObject);
		registerKeyword(std::regex("names"), commonItems::ignoreObject);
		registerKeyword(std::regex("provinces"), [this](const std::string& unused, std::istream& theStream)
			{
				auto equals = getNextToken(theStream);
				commonItems::intList theProvinces(theStream);
				auto provincesVector = theProvinces.getInts();
				std::for_each(provincesVector.begin(), provincesVector.end(), [this](const int& province)
					{
						provinces.insert(province);
					}
				);
			}
		);

		parseStream(theStream);
	}


	colonialRegions* colonialRegions::instance = nullptr;



	colonialRegions::colonialRegions()
	{
		LOG(LogLevel::Info) << "Parsing EU4 colonial regions";

		registerKeyword(std::regex("colonial_\\w+"), [this](const std::string& regionName, std::istream& theStream)
			{
				colonialRegion newRegion(theStream);
				auto provinces = newRegion.getProvinces();
				std::for_each(provinces.begin(), provinces.end(), [this, regionName](const int& province)
					{
						provinceToColonialRegions.insert(std::make_pair(province, regionName));
					}
				);
			}
		);
		
		parseFile(Configuration::getEU4Path() + "/common/colonial_regions/00_colonial_regions.txt");

		for (auto mod: Configuration::getEU4Mods())
		{
			set<string> filenames;
			Utils::GetAllFilesInFolder(mod + "/common/colonial_regions/", filenames);
			for (auto filename: filenames)
			{
				string modRegionsFile(mod + "/common/colonial_regions/" + filename);
				if (Utils::DoesFileExist(modRegionsFile))
				{
					parseFile(modRegionsFile);
				}
			}
		}
	}


	bool colonialRegions::ProvinceIsInRegion(int province, const std::string& region) const
	{
		auto mapping = provinceToColonialRegions.find(province);
		if (mapping != provinceToColonialRegions.end())
		{
			return (mapping->second == region);
		}
		else
		{
			return false;
		}
	}
}