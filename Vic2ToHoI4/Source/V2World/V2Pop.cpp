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



#include "V2Pop.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Issues.h"



Vic2::Pop::Pop(const std::string& typeString, std::istream& theStream):
	type(typeString)
{
	registerKeyword(std::regex("size"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleInt sizeInt(theStream);
		size = sizeInt.getInt();
	});
	registerKeyword(std::regex("literacy"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleDouble sizeInt(theStream);
		literacy = sizeInt.getDouble();
	});
	registerKeyword(std::regex("con"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleDouble sizeInt(theStream);
		consciousness = sizeInt.getDouble();
	});
	registerKeyword(std::regex("mil"), [this](const std::string& unused, std::istream& theStream)
	{
		commonItems::singleDouble sizeInt(theStream);
		militancy = sizeInt.getDouble();
	});
	registerKeyword(std::regex("issues"), [this](const std::string& unused, std::istream& theStream)
	{
		auto equals = getNextToken(theStream);
		auto openBrace = getNextToken(theStream);

		auto possibleIssue = getNextToken(theStream);
		while (possibleIssue != "}")
		{
			auto equals = getNextToken(theStream);
			auto issueSupport = getNextToken(theStream);
			if (issueSupport->substr(0,1) == "\"")
			{
				issueSupport = issueSupport->substr(1, issueSupport->size() - 2);
			}

			std::string issueName = issuesInstance.getIssueName(std::stoi(*possibleIssue));
			popIssues.insert(std::make_pair(issueName, std::stof(*issueSupport)));

			possibleIssue = getNextToken(theStream);
		}
	});

	registerKeyword(std::regex("id"), commonItems::ignoreString);
	registerKeyword(std::regex("money"), commonItems::ignoreString);
	registerKeyword(std::regex("bank"), commonItems::ignoreString);
	registerKeyword(std::regex("con_factor"), commonItems::ignoreString);
	registerKeyword(std::regex("demoted"), commonItems::ignoreString);
	registerKeyword(std::regex("luxury_needs"), commonItems::ignoreString);
	registerKeyword(std::regex("everyday_needs"), commonItems::ignoreString);
	registerKeyword(std::regex("life_needs"), commonItems::ignoreString);
	registerKeyword(std::regex("random"), commonItems::ignoreString);
	registerKeyword(std::regex("movement_issue"), commonItems::ignoreString);
	registerKeyword(std::regex("production_type"), commonItems::ignoreString);
	registerKeyword(std::regex("last_spending"), commonItems::ignoreString);
	registerKeyword(std::regex("current_producing"), commonItems::ignoreString);
	registerKeyword(std::regex("percent_afforded"), commonItems::ignoreString);
	registerKeyword(std::regex("percent_sold_domestic"), commonItems::ignoreString);
	registerKeyword(std::regex("percent_sold_export"), commonItems::ignoreString);
	registerKeyword(std::regex("leftover"), commonItems::ignoreString);
	registerKeyword(std::regex("throttle"), commonItems::ignoreString);
	registerKeyword(std::regex("needs_cost"), commonItems::ignoreString);
	registerKeyword(std::regex("production_income"), commonItems::ignoreString);
	registerKeyword(std::regex("promoted"), commonItems::ignoreString);
	registerKeyword(std::regex("days_of_loss"), commonItems::ignoreString);
	registerKeyword(std::regex("converted"), commonItems::ignoreString);
	registerKeyword(std::regex("local_migration"), commonItems::ignoreString);
	registerKeyword(std::regex("colonial_migration"), commonItems::ignoreString);
	registerKeyword(std::regex("external_migration"), commonItems::ignoreString);
	registerKeyword(std::regex("movement_tag"), commonItems::ignoreString);
	registerKeyword(std::regex("assimilated"), commonItems::ignoreString);
	registerKeyword(std::regex("ideology"), commonItems::ignoreObject);
	registerKeyword(std::regex("stockpile"), commonItems::ignoreObject);
	registerKeyword(std::regex("need"), commonItems::ignoreObject);
	registerKeyword(std::regex("size_changes"), commonItems::ignoreObject);
	registerKeyword(std::regex("faction"), commonItems::ignoreObject);
	registerKeyword(std::regex("[a-z\\_]+"), [this](const std::string& cultureString, std::istream& theStream)
	{
		if (culture == "no_culture")
		{
			culture = cultureString;
			commonItems::singleString religionString(theStream);
			religion = religionString.getString();
		}
		else
		{
			LOG(LogLevel::Debug) << "Unknown token while parsing pop: " << cultureString;
		}
	});

	parseStream(theStream);
}


float Vic2::Pop::getIssue(const std::string& issueName) const
{
	auto issue = popIssues.find(issueName);
	if (issue != popIssues.end())
	{
		return issue->second;
	}
	else
	{
		return 0.0f;
	}
}