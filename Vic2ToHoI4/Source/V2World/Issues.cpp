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



#include "Issues.h"
#include "../Configuration.h"
#include "ParserHelpers.h"
#include <vector>



Vic2::issues Vic2::issuesInstance;


class issueHelper: commonItems::parser
{
	public:
		issueHelper(std::istream& theStream);

		std::vector<std::string> getIssues() { return issues; }

	private:
		std::vector<std::string> issues;
};


class partiesIssueHelper: commonItems::parser
{
	public:
		partiesIssueHelper(std::istream& theStream);

		std::vector<issueHelper> getIssues() { return issues; }

	private:
		std::vector<issueHelper> issues;
};



void Vic2::issues::instantiate()
{
	int issueNum = 1;

	registerKeyword(std::regex("party_issues"), [this, &issueNum](const std::string& category, std::istream& theStream)
	{
		partiesIssueHelper helperHelper(theStream);
		for (auto helper: helperHelper.getIssues())
		{
			for (auto name: helper.getIssues())
			{
				issueNames.insert(make_pair(issueNum, name));
				issueNum++;
			}
		}
	});
	registerKeyword(std::regex("[A-Za-z_]+"), [this, &issueNum](const std::string& category, std::istream& theStream)
	{
		issueHelper helper(theStream);
		for (auto name: helper.getIssues())
		{
			issueNames.insert(make_pair(issueNum, name));
			issueNum++;
		}
	});

	parseFile(Configuration::getV2Path() + "/common/issues.txt");
}


std::string Vic2::issues::getIssueName(int num) const
{
	auto name = issueNames.find(num);
	if (name != issueNames.end())
	{
		return name->second;
	}
	else
	{
		return "";
	}
}


issueHelper::issueHelper(std::istream& theStream)
{
	registerKeyword(std::regex("[A-Za-z_]+"), [this](const std::string& theIssue, std::istream& theStream)
	{
		issues.push_back(theIssue);
		commonItems::ignoreObject(theIssue, theStream);
	});

	parseStream(theStream);
}


partiesIssueHelper::partiesIssueHelper(std::istream& theStream)
{
	registerKeyword(std::regex("[A-Za-z_]+"), [this](const std::string& theIssue, std::istream& theStream)
	{
		issueHelper helper(theStream);
		issues.push_back(helper);
	});

	parseStream(theStream);
}