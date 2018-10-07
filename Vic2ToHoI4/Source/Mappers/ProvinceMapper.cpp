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



#include "ProvinceMapper.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "../Configuration.h"
#include <fstream>



provinceMapper theProvinceMapper;



class mapping: commonItems::parser
{
	public:
		mapping(std::istream& theStream);

		auto getVic2Nums() const { return Vic2Nums; }
		auto getHoI4Nums() const { return HoI4Nums; }

	private:
		std::vector<int> Vic2Nums;
		std::vector<int> HoI4Nums;
};


mapping::mapping(std::istream& theStream)
{
	registerKeyword(std::regex("vic2"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt provinceNum(theStream);
		Vic2Nums.push_back(provinceNum.getInt());
	});
	registerKeyword(std::regex("hoi4"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt provinceNum(theStream);
		HoI4Nums.push_back(provinceNum.getInt());
	});

	parseStream(theStream);

	if (Vic2Nums.size() == 0)
	{
		Vic2Nums.push_back(0);
	}
	if (HoI4Nums.size() == 0)
	{
		HoI4Nums.push_back(0);
	}
}


class versionMappings: commonItems::parser
{
	public:
		versionMappings(std::istream& theStream);

		auto getVic2ToHoI4Mapping() const { return Vic2ToHoI4ProvinceMap; }
		auto getHoI4ToVic2Mapping() const { return HoI4ToVic2ProvinceMap; }

	private:
		void insertIntoHoI4ToVic2ProvinceMap(const std::vector<int>& Vic2Nums, const std::vector<int>& HoI4nums);
		void insertIntoVic2ToHoI4ProvinceMap(const std::vector<int>& Vic2Nums, const std::vector<int>& HoI4nums);

		HoI4ToVic2ProvinceMapping HoI4ToVic2ProvinceMap;
		Vic2ToHoI4ProvinceMapping Vic2ToHoI4ProvinceMap;
};


versionMappings::versionMappings(std::istream& theStream)
{
	registerKeyword(std::regex("link"), [this](const std::string& unused, std::istream& theStream){
		mapping theMapping(theStream);
		insertIntoHoI4ToVic2ProvinceMap(theMapping.getVic2Nums(), theMapping.getHoI4Nums());
		insertIntoVic2ToHoI4ProvinceMap(theMapping.getVic2Nums(), theMapping.getHoI4Nums());
	});

	parseStream(theStream);
}


void versionMappings::insertIntoHoI4ToVic2ProvinceMap(const std::vector<int>& Vic2Nums, const std::vector<int>& HoI4Nums)
{
	for (auto num: HoI4Nums)
	{
		if (num != 0)
		{
			HoI4ToVic2ProvinceMap.insert(make_pair(num, Vic2Nums));
		}
	}
}


void versionMappings::insertIntoVic2ToHoI4ProvinceMap(const std::vector<int>& Vic2Nums, const std::vector<int>& HoI4Nums)
{
	for (auto num: Vic2Nums)
	{
		if (num != 0)
		{
			Vic2ToHoI4ProvinceMap.insert(make_pair(num, HoI4Nums));
		}
	}
}


void provinceMapper::initialize()
{
	bool gotMappings = false;
	registerKeyword(std::regex("\\d\\.\\d\\.\\d"), [this, &gotMappings](const std::string& version, std::istream& theStream){
		HoI4::Version currentVersion(version);
		if ((theConfiguration.getHOI4Version() >= currentVersion) && !gotMappings)
		{
			LOG(LogLevel::Debug) << "Using version " << version << " mappings";
			versionMappings thisVersionsMappings(theStream);
			HoI4ToVic2ProvinceMap = thisVersionsMappings.getHoI4ToVic2Mapping();
			Vic2ToHoI4ProvinceMap = thisVersionsMappings.getVic2ToHoI4Mapping();
			gotMappings = true;
		}
		else
		{
			commonItems::ignoreItem(version, theStream);
		}
	});

	LOG(LogLevel::Info) << "Parsing province mappings";
	bool mapped = false;
	for (auto mod: theConfiguration.getVic2Mods())
	{
		if (Utils::DoesFileExist(mod + "_province_mappings.txt"))
		{
			parseFile(mod + "_province_mappings.txt");
			mapped = true;
			break;
		}
	}
	if (!mapped)
	{
		parseFile("province_mappings.txt");
	}

	checkAllHoI4ProvinesMapped();
}


void provinceMapper::checkAllHoI4ProvinesMapped() const
{
	std::ifstream definitions(theConfiguration.getHoI4Path() + "/map/definition.csv");
	if (!definitions.is_open())
	{
		LOG(LogLevel::Error) << "Could not open " << theConfiguration.getHoI4Path() << "/map/definition.csv";
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		auto provNum = getNextProvinceNumFromFile(definitions);
		if (!provNum)
		{
			break;
		}

		verifyProvinceIsMapped(*provNum);
	}

	definitions.close();
}


std::optional<int> provinceMapper::getNextProvinceNumFromFile(std::ifstream& definitions) const
{
	std::string line;
	getline(definitions, line);
	int pos = line.find_first_of(';');
	if (pos != std::string::npos)
	{
		return stoi(line.substr(0, pos));
	}
	else
	{
		return {};
	}
}


void provinceMapper::verifyProvinceIsMapped(int provNum) const
{
	if (provNum != 0)
	{
		auto num = HoI4ToVic2ProvinceMap.find(provNum);
		if (num == HoI4ToVic2ProvinceMap.end())
		{
			LOG(LogLevel::Warning) << "No mapping for HoI4 province " << provNum;
		}
	}
}


std::optional<std::vector<int>> provinceMapper::getVic2ToHoI4ProvinceMapping(int Vic2Province) const
{
	if (auto mapping = Vic2ToHoI4ProvinceMap.find(Vic2Province); mapping != Vic2ToHoI4ProvinceMap.end())
	{
		return mapping->second;
	}
	else
	{
		return std::nullopt;
	}
}


std::optional<std::vector<int>> provinceMapper::getHoI4ToVic2ProvinceMapping(int HoI4Province) const
{
	if (auto mapping = HoI4ToVic2ProvinceMap.find(HoI4Province); mapping != HoI4ToVic2ProvinceMap.end())
	{
		return mapping->second;
	}
	else
	{
		return std::nullopt;
	}
}