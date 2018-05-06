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



#include "Relations.h"
#include "ParserHelpers.h"



Vic2::Relations::Relations(const std::string& theTag, std::istream& theStream):
	tag(theTag)
{
	registerKeyword(std::regex("value"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt valueInt(theStream);
		value = valueInt.getInt();
	});
	registerKeyword(std::regex("level"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt levelInt(theStream);
		level = levelInt.getInt();
	});
	registerKeyword(std::regex("military_access"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString stateString(theStream);
		militaryAccess = (stateString.getString() == "yes");
	});
	registerKeyword(std::regex("last_send_diplomat"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString dateString(theStream);
		lastSentDiplomat = date(dateString.getString());
	});
	registerKeyword(std::regex("last_war"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString dateString(theStream);
		lastWar = date(dateString.getString());
	});
	registerKeyword(std::regex("truce_until"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleString dateString(theStream);
		truceUntil = date(dateString.getString());
	});
	registerKeyword(std::regex("[A-Za-z0-9_]+"), commonItems::ignoreItem);

	parseStream(theStream);
}