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



#ifndef COUNTRYMAPPING_H
#define COUNTRYMAPPING_H



#include "newParser.h"
#include <map>
#include <optional>
#include <string>
#include <vector>



namespace Vic2
{
class World;
}



class CountryMapper: commonItems::parser
{
	public:
		explicit CountryMapper(const Vic2::World* srcWorld);

		std::optional<std::string> getHoI4Tag(const std::string& V2Tag) const;
		std::optional<std::string> getVic2Tag(const std::string& HoI4Tag) const;

	private:
		CountryMapper(const CountryMapper&) = delete;
		CountryMapper& operator=(const CountryMapper&) = delete;

		void readRules();

		void createMappings(const Vic2::World* srcWorld);
		void resetMappingData();
		void makeOneMapping(const std::string& Vic2Tag);
		bool mapToFirstUnusedVic2Tag(const std::vector<std::string>& possibleVic2Tags, const std::string& EU4Tag);
		std::string generateNewHoI4Tag(const std::string& Vic2Tag);
		void mapToNewTag(const std::string& Vic2Tag, const std::string& HoI4Tag);
		void logMapping(const std::string& sourceTag, const std::string& targetTag, const std::string& reason) const;
		bool tagIsAlreadyAssigned(const std::string& HoI4Tag) const;

		std::map<std::string, std::vector<std::string>> Vic2TagToHoI4TagsRules;
		std::map<std::string, std::string> V2TagToHoI4TagMap;
		std::map<std::string, std::string> HoI4TagToV2TagMap;

		char generatedHoI4TagPrefix;
		int generatedHoI4TagSuffix;
};



#endif // COUNTRYMAPPING_H