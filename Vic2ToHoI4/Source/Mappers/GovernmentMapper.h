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



#ifndef GOVERNMENTMAPPER_H
#define GOVERNMENTMAPPER_H



#include "../HOI4World/HOI4Ideology.h"
#include "newParser.h"
#include <memory>
#include <set>
#include <string>
#include <vector>



namespace Vic2
{
class Country;
}



typedef struct governmentMapping
{
	string vic2Government;
	string HoI4GovernmentIdeology;
	string HoI4LeaderIdeology;
	string rulingPartyRequired;
} governmentMapping;



typedef struct partyMapping
{
	string rulingIdeology;
	string vic2Ideology;
	string supportedIdeology;
} partyMapping;


class governmentMapper: commonItems::parser
{
	public:
		governmentMapper();

		std::string getIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology) const;
		std::string getLeaderIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology) const;
		std::string getExistingIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology, const std::set<std::string>& majorIdeologies, const std::map<std::string, HoI4Ideology*>& ideologies) const;
		std::string getExistingLeaderIdeologyForCountry(const Vic2::Country* country, const std::string& Vic2RulingIdeology, const std::set<std::string>& majorIdeologies, const std::map<std::string, HoI4Ideology*>& ideologies) const;
		std::string getSupportedIdeology(const std::string& rulingIdeology, const std::string& Vic2Ideology, const std::set<std::string>& majorIdeologies) const;

		auto getGovernmentMappings() const { return governmentMap; }

	private:
		governmentMapper(const governmentMapper&) = delete;
		governmentMapper& operator=(const governmentMapper&) = delete;

		bool governmentMatches(const governmentMapping& mapping, const std::string& government) const;
		bool rulingIdeologyMatches(const governmentMapping& mapping, const std::string& rulingIdeology) const;
		bool ideologyIsValid(const governmentMapping& mapping, const std::set<std::string>& majorIdeologies, const std::map<std::string, HoI4Ideology*>& ideologies) const;

		std::vector<governmentMapping> governmentMap;
		std::vector<partyMapping> partyMap;
};



#endif // GOVERNMENTMAPPER_H