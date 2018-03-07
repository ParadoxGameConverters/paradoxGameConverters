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



#include <istream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include "ColonialTagsMapper.h"
#include "newParser.h"



namespace EU4
{
	class Country;
	class world;
}
class V2Country;




namespace mappers
{
	class CountryMapping: commonItems::parser
	{
		public:
			CountryMapping(std::istream& theStream);

			std::string getEU4Tag() const { return EU4Tag; }
			std::string getVic2Tag() const { return Vic2Tag; }

		private:
			std::string EU4Tag;
			std::string Vic2Tag;
	};


	class CountryMappings: commonItems::parser
	{
		public:
			static void createMappings(const EU4::world& srcWorld, const std::map<std::string, V2Country*>& Vic2Countries)
			{
				getInstance()->CreateMappings(srcWorld, Vic2Countries);
			}

			static std::string getVic2Tag(const std::string& EU4Tag)
			{
				return getInstance()->GetV2Tag(EU4Tag);
			}

			static std::optional<std::string> getCK2Title(const std::string& EU4Tag, const std::string& countryName, const std::set<std::string>& availableFlags)
			{
				return getInstance()->GetCK2Title(EU4Tag, countryName, availableFlags);
			}

		private:
			static CountryMappings* instance;
			static CountryMappings* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new CountryMappings();
				}

				return instance;
			}

			CountryMappings();
			void readRules();
			void getAvailableFlags();

			void CreateMappings(const EU4::world& srcWorld, const std::map<std::string, V2Country*>& Vic2Countries);
			bool isPotentialColonialReplacement(const std::pair<std::string, std::shared_ptr<EU4::Country>>& country);
			bool tagIsAlphaDigitDigit(const std::string& tag);
			void makeOneMapping(std::shared_ptr<EU4::Country> country, const std::map<std::string, V2Country*>& Vic2Countries);
			std::map<std::string, std::string>::iterator ifValidGetCK2MappingRule(const std::shared_ptr<EU4::Country> country, std::map<std::string, std::string>::iterator mappingRule);
			bool mapToExistingVic2Country(const std::string& possibleVic2Tags, const std::map<std::string, V2Country*>& Vic2Countries, const std::string& EU4Tag);
			bool mapToFirstUnusedVic2Tag(const std::string& possibleVic2Tags, const std::string& EU4Tag);
			std::string generateNewTag();
			void mapToNewTag(const std::string& EU4Tag, const std::string& Vic2Tag);
			bool attemptColonialReplacement(std::shared_ptr<EU4::Country> country, const EU4::world& srcWorld, const std::map<std::string, V2Country*>& Vic2Countries);
			bool capitalInRightEU4Region(const mappers::colonyStruct& colony, int EU4Capital);
			bool capitalInRightVic2Region(const mappers::colonyStruct& colony, int Vic2Capital, const EU4::world& srcWorld, const std::string& EU4Tag);
			bool inCorrectCultureGroup(const mappers::colonyStruct& colony, const std::string& primaryCulture);
			bool tagIsAvailable(const mappers::colonyStruct& colony, const std::map<std::string, V2Country*>& Vic2Countries);
			void logMapping(const std::string& EU4Tag, const std::string& V2Tag, const std::string& reason);
			bool tagIsAlreadyAssigned(const std::string& Vic2Tag);

			const std::string GetV2Tag(const std::string& EU4Tag) const;

			std::optional<std::string> GetCK2Title(const std::string& EU4Tag, const std::string& countryName, const std::set<std::string>& availableFlags);

			std::map<std::string, std::string> EU4TagToV2TagsRules;
			std::map<std::string, std::string> EU4TagToV2TagMap;
			std::map<std::string, std::string> V2TagToEU4TagMap;

			char generatedV2TagPrefix;
			int generatedV2TagSuffix;

			std::set<std::string> availableFlags;
	};
}



#endif // COUNTRYMAPPING_H