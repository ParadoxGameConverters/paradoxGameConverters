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



#include "MergeRules.h"
#include "ParserHelpers.h"



class MergeRule: commonItems::parser
{
	public:
		MergeRule(std::istream& theStream);

		bool isEnabled() const { return enabled; }
		std::string getMaster() const { return master; }
		std::vector<std::string> getSlaves() const { return slaves; }

	private:
		bool enabled = false;
		std::string master = "";
		std::vector<std::string> slaves;
};


MergeRules::MergeRules()
{
	registerKeyword(std::regex("[A-Za-z_]+"), [this](const std::string& ruleName, std::istream& theStream){
		MergeRule theRule(theStream);
		if (theRule.isEnabled())
		{
			rules.insert(make_pair(theRule.getMaster(), theRule.getSlaves()));
		}
	});

	parseFile("merge_nations.txt");
}


MergeRule::MergeRule(std::istream& theStream)
{
	registerKeyword(std::regex("merge"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString enabledString(theStream);
		enabled = (enabledString.getString() == "yes");
	});
	registerKeyword(std::regex("master"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString masterString(theStream);
		master = masterString.getString();
	});
	registerKeyword(std::regex("slave"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString slaveString(theStream);
		slaves.push_back(slaveString.getString());
	});

	parseStream(theStream);
}