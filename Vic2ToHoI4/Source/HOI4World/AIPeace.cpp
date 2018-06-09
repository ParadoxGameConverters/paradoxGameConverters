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



#include "AIPeace.h"
#include "ParserHelpers.h"



HoI4::AIPeace::AIPeace(const std::string& theName, std::istream& theStream):
	name(theName)
{
	registerKeyword(std::regex("enable"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem enableString(theStream);
		enable = enableString.getString();
	});
	registerKeyword(std::regex("annex_randomness"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt randomnessInt(theStream);
		annexRandomness = randomnessInt.getInt();
	});
	registerKeyword(std::regex("liberate_randomness"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt randomnessInt(theStream);
		liberateRandomness = randomnessInt.getInt();
	});
	registerKeyword(std::regex("puppet_randomness"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt randomnessInt(theStream);
		puppetRandomness = randomnessInt.getInt();
	});
	registerKeyword(std::regex("take_states_randomness"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt randomnessInt(theStream);
		takeStatesRandomness = randomnessInt.getInt();
	});
	registerKeyword(std::regex("force_government_randomness"), [this](const std::string& unused, std::istream& theStream){
		commonItems::singleInt randomnessInt(theStream);
		forceGovernmentRandomness = randomnessInt.getInt();
	});
	registerKeyword(std::regex("annex"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem annexString(theStream);
		annex = annexString.getString();
	});
	registerKeyword(std::regex("liberate"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem liberateString(theStream);
		liberate = liberateString.getString();
	});
	registerKeyword(std::regex("puppet"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem puppetString(theStream);
		puppet = puppetString.getString();
	});
	registerKeyword(std::regex("take_states"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem takeStatesString(theStream);
		takeStates = takeStatesString.getString();
	});
	registerKeyword(std::regex("force_government"), [this](const std::string& unused, std::istream& theStream){
		commonItems::stringOfItem forceGovernmentString(theStream);
		forceGovernment = forceGovernmentString.getString();
	});

	parseStream(theStream);
}


std::ostream& HoI4::operator<<(std::ostream& outStream, const HoI4::AIPeace& outPeace)
{
	outStream << outPeace.name << " = {\n";
	if (outPeace.enable != "")
	{
		outStream << "	enable " << outPeace.enable << "\n";
	}

	outStream << "	annex_randomness = " << outPeace.annexRandomness << "\n";
	outStream << "	liberate_randomness = " << outPeace.liberateRandomness << "\n";
	outStream << "	puppet_randomness = " << outPeace.puppetRandomness << "\n";
	outStream << "	take_states_randomness = " << outPeace.takeStatesRandomness << "\n";
	outStream << "	force_government_randomness = " << outPeace.forceGovernmentRandomness << "\n";

	outStream << "	annex " << outPeace.annex << "\n";
	outStream << "	liberate " << outPeace.liberate << "\n";
	outStream << "	puppet " << outPeace.puppet << "\n";
	outStream << "	take_states " << outPeace.takeStates << "\n";
	outStream << "	force_government " << outPeace.forceGovernment << "\n";

	outStream << "}";

	return outStream;
}