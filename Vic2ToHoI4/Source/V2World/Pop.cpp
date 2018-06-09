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



#include "Pop.h"
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
		auto equals = getNextTokenWithoutMatching(theStream);
		auto openBrace = getNextTokenWithoutMatching(theStream);

		auto possibleIssue = getNextTokenWithoutMatching(theStream);
		while (possibleIssue != "}")
		{
			auto equals = getNextTokenWithoutMatching(theStream);
			auto issueSupport = getNextTokenWithoutMatching(theStream);

			std::string issueName = issuesInstance.getIssueName(std::stoi(*possibleIssue));
			popIssues.insert(std::make_pair(issueName, std::stof(*issueSupport)));

			possibleIssue = getNextTokenWithoutMatching(theStream);
		}
	});
	registerKeyword(std::regex("id"), commonItems::ignoreItem);
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
			commonItems::ignoreItem(cultureString, theStream);
		}
	});
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), commonItems::ignoreItem);

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