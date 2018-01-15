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



#include "Object.h"
#include <map>
#include <optional>
#include <string>
using namespace std;



class Object;
class V2World;



class CountryMapper
{
	public:
		static void createMappings(const V2World* srcWorld)
		{
			getInstance()->CreateMappings(srcWorld);
		}

		static optional<string> getHoI4Tag(const string& V2Tag)
		{
			return getInstance()->GetHoI4Tag(V2Tag);
		}

		static optional<string> getVic2Tag(const string& HoI4Tag)
		{
			return getInstance()->GetVic2Tag(HoI4Tag);
		}

	private:
		static CountryMapper* instance;
		static CountryMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new CountryMapper();
			}

			return instance;
		}
		CountryMapper();

		CountryMapper(const CountryMapper&) = delete;
		CountryMapper& operator=(const CountryMapper&) = delete;

		void readRules();
		vector<shared_ptr<Object>> getRules() const;
		void importRule(shared_ptr<Object> rule);

		void CreateMappings(const V2World* srcWorld);
		void resetMappingData();
		void makeOneMapping(const string& Vic2Tag);
		bool mapToFirstUnusedVic2Tag(const vector<string>& possibleVic2Tags, const string& EU4Tag);
		string generateNewHoI4Tag(const string& Vic2Tag);
		void mapToNewTag(const string& Vic2Tag, const string& HoI4Tag);
		void LogMapping(const string& sourceTag, const string& targetTag, const string& reason) const;
		bool tagIsAlreadyAssigned(const string& HoI4Tag) const;

		optional<string> GetHoI4Tag(const string& V2Tag) const;
		optional<string> GetVic2Tag(const string& HoI4Tag) const;

		map<string, vector<string>> Vic2TagToHoI4TagsRules;
		map<string, string> V2TagToHoI4TagMap;
		map<string, string> HoI4TagToV2TagMap;

		char generatedHoI4TagPrefix;
		int generatedHoI4TagSuffix;
};



#endif // COUNTRYMAPPING_H