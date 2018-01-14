/*Copyright (c) 2017 The Paradox Game Converters Project

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



#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "ColonialTagsMapper.h"
using namespace std;



class Object;
class EU4World;
class EU4Country;
class V2Country;




class CountryMapping
{
	public:
		static void createMappings(const EU4World& srcWorld, const map<string, V2Country*>& Vic2Countries)
		{
			getInstance()->CreateMappings(srcWorld, Vic2Countries);
		}

		static string getVic2Tag(const string& EU4Tag)
		{
			return getInstance()->GetV2Tag(EU4Tag);
		}

		static string getCK2Title(const string& EU4Tag, const string& countryName, const set<string>& availableFlags)
		{
			return getInstance()->GetCK2Title(EU4Tag, countryName, availableFlags);
		}

	private:
		static CountryMapping* instance;
		static CountryMapping* getInstance()
		{
			if (instance == NULL)
			{
				instance = new CountryMapping();
			}

			return instance;
		}

		CountryMapping();
		void readRules();
		vector<shared_ptr<Object>> getRules();
		void importRule(shared_ptr<Object> rule);
		void readEU4Regions();
		vector<shared_ptr<Object>> parseEU4RegionsFiles();
		void readVic2Regions();
		shared_ptr<Object> parseVic2RegionsFile();
		void getAvailableFlags();

		void CreateMappings(const EU4World& srcWorld, const map<string, V2Country*>& Vic2Countries);
		bool isPotentialColonialReplacement(const pair<string, EU4Country*>& country);
		bool tagIsAlphaDigitDigit(const string& tag);
		void makeOneMapping(EU4Country* country, const map<string, V2Country*>& Vic2Countries);
		map<string, vector<string>>::iterator ifValidGetCK2MappingRule(const EU4Country* country, map<string, vector<string>>::iterator mappingRule);
		bool mapToExistingVic2Country(const vector<string>& possibleVic2Tags, const map<string, V2Country*>& Vic2Countries, const string& EU4Tag);
		bool mapToFirstUnusedVic2Tag(const vector<string>& possibleVic2Tags, const string& EU4Tag);
		string generateNewTag();
		void mapToNewTag(const string& EU4Tag, const string& Vic2Tag);
		bool attemptColonialReplacement(EU4Country* country, const EU4World& srcWorld, const map<string, V2Country*>& Vic2Countries);
		bool capitalInRightEU4Region(const colonyStruct& colony, int EU4Capital);
		bool capitalInRightVic2Region(const colonyStruct& colony, int Vic2Capital, const EU4World& srcWorld, const string& EU4Tag);
		bool inCorrectCultureGroup(const colonyStruct& colony, const string& primaryCulture);
		bool tagIsAvailable(const colonyStruct& colony, const map<string, V2Country*>& Vic2Countries);
		void logMapping(const string& EU4Tag, const string& V2Tag, const string& reason);
		bool tagIsAlreadyAssigned(const string& Vic2Tag);

		const string GetV2Tag(const string& EU4Tag) const;

		string GetCK2Title(const string& EU4Tag, const string& countryName, const set<string>& availableFlags);

		map<string, vector<string>> EU4TagToV2TagsRules;
		map<string, string> EU4TagToV2TagMap;
		map<string, string> V2TagToEU4TagMap;
		map<string, set<int>> EU4ColonialRegions;
		map<string, set<int>> Vic2Regions;

		char generatedV2TagPrefix;
		int generatedV2TagSuffix;

		set<string> availableFlags;
};



#endif // COUNTRYMAPPING_H