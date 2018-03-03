/*Copyright(c) 2018 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */



#include "CountryHistory.h"



EU4::countryHistory::countryHistory(std::istream& theStream):
	items()
{
	auto ignore = getNextToken(theStream);

	registerKeyword(std::regex("government"), commonItems::ignoreString);
	registerKeyword(std::regex("government_rank"), commonItems::ignoreString);
	registerKeyword(std::regex("mercantilism"), commonItems::ignoreString);
	registerKeyword(std::regex("technology_group"), commonItems::ignoreString);
	registerKeyword(std::regex("historical_friend"), commonItems::ignoreString);
	registerKeyword(std::regex("historical_rival"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_type"), commonItems::ignoreString);
	registerKeyword(std::regex("primary_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("religion"), commonItems::ignoreString);
	registerKeyword(std::regex("religious_school"), commonItems::ignoreString);
	registerKeyword(std::regex("secondary_religion"), commonItems::ignoreString);
	registerKeyword(std::regex("add_harmonized_religion"), commonItems::ignoreString);
	registerKeyword(std::regex("add_accepted_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("capital"), commonItems::ignoreString);
	registerKeyword(std::regex("national_focus"), commonItems::ignoreString);
	registerKeyword(std::regex("fixed_capital"), commonItems::ignoreString);
	registerKeyword(std::regex("mercantilism"), commonItems::ignoreString);
	registerKeyword(std::regex("unlock_cult"), commonItems::ignoreString);
	registerKeyword(std::regex("elector"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_type"), commonItems::ignoreString);
	registerKeyword(std::regex("\\d+\\.\\d+\\.\\d+"), [this](const std::string& date, std::istream& theStream)
		{
			countryHistoryDate theDate(theStream);
			for (auto& item: theDate.getItems())
			{
				items.emplace_back(std::dynamic_pointer_cast<historyItem>(item));
			}
		}
	);

	parseStream(theStream);
}


EU4::countryHistoryDate::countryHistoryDate(std::istream& theStream):
	items()
{
	registerKeyword(std::regex("monarch"), commonItems::ignoreObject);
	registerKeyword(std::regex("queen"), commonItems::ignoreObject);
	registerKeyword(std::regex("heir"), commonItems::ignoreObject);
	registerKeyword(std::regex("leader"), [this](const std::string& date, std::istream& theStream)
		{
			shared_ptr<historyLeader> newLeader = make_unique<historyLeader>(theStream);
			items.emplace_back(newLeader);
		}
	);
	registerKeyword(std::regex("changed_country_mapcolor_from"), commonItems::ignoreObject);
	registerKeyword(std::regex("government"), commonItems::ignoreString);
	registerKeyword(std::regex("religion"), commonItems::ignoreString);
	registerKeyword(std::regex("add_accepted_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("adm_tech"), commonItems::ignoreString);
	registerKeyword(std::regex("dip_tech"), commonItems::ignoreString);
	registerKeyword(std::regex("mil_tech"), commonItems::ignoreString);
	registerKeyword(std::regex("capital"), commonItems::ignoreString);
	registerKeyword(std::regex("changed_tag_from"), commonItems::ignoreString);
	registerKeyword(std::regex("changed_country_name_from"), commonItems::ignoreString);
	registerKeyword(std::regex("changed_country_adjective_from"), commonItems::ignoreString);
	registerKeyword(std::regex("set_country_flag"), commonItems::ignoreString);
	registerKeyword(std::regex("clr_country_flag"), commonItems::ignoreString);
	registerKeyword(std::regex("set_global_flag"), commonItems::ignoreString);
	registerKeyword(std::regex("union"), commonItems::ignoreString);
	registerKeyword(std::regex("primary_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("religion"), commonItems::ignoreString);
	registerKeyword(std::regex("government_rank"), commonItems::ignoreString);
	registerKeyword(std::regex("decision"), commonItems::ignoreString);
	registerKeyword(std::regex("remove_accepted_culture"), commonItems::ignoreString);
	registerKeyword(std::regex("mercantilism"), commonItems::ignoreString);
	registerKeyword(std::regex("elector"), commonItems::ignoreString);
	registerKeyword(std::regex("revolution_target"), commonItems::ignoreString);
	registerKeyword(std::regex("religious_school"), commonItems::ignoreString);
	registerKeyword(std::regex("technology_group"), commonItems::ignoreString);
	registerKeyword(std::regex("unit_type"), commonItems::ignoreString);

	parseStream(theStream);
}


EU4::historyLeader::historyLeader(std::istream& theStream)
{
	type = "leader";
	theLeader = std::make_unique<EU4::leader>(EU4::leader(theStream));
}