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



#include "Party.h"
#include "ParserHelpers.h"



Vic2::Party::Party(std::istream& theStream)
{
	registerKeyword(std::regex("name"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString nameString(theStream);
		name = nameString.getString();
	});
	registerKeyword(std::regex("ideology"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString ideologyString(theStream);
		ideology = ideologyString.getString();
	});
	registerKeyword(std::regex("economic_policy"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString policyString(theStream);
		economic_policy = policyString.getString();
	});
	registerKeyword(std::regex("trade_policy"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString policyString(theStream);
		trade_policy = policyString.getString();
	});
	registerKeyword(std::regex("religious_policy"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString policyString(theStream);
		religious_policy = policyString.getString();
	});
	registerKeyword(std::regex("citizenship_policy"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString policyString(theStream);
		citizenship_policy = policyString.getString();
	});
	registerKeyword(std::regex("war_policy"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString policyString(theStream);
		warPolicy = policyString.getString();
	});
	registerKeyword(std::regex("[A-Za-z0-9\\_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}
