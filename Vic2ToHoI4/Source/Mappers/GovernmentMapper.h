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



#ifndef GOVERNMENTMAPPER_H
#define GOVERNMENTMAPPER_H



#include <memory>
#include <set>
#include <string>
#include <vector>
using namespace std;



class Object;
class V2Country;



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


class governmentMapper
{
	public:
		static string getIdeologyForCountry(const V2Country* country, const string& Vic2RulingIdeology)
		{
			return getInstance()->GetIdeologyForCountry(country, Vic2RulingIdeology);
		}

		static string getLeaderIdeologyForCountry(const V2Country* country, const string& Vic2RulingIdeology)
		{
			return getInstance()->GetLeaderIdeologyForCountry(country, Vic2RulingIdeology);
		}

		static string getSupportedIdeology(const string& rulingIdeology, const string& Vic2Ideology)
		{
			return getInstance()->GetSupportedIdeology(rulingIdeology, Vic2Ideology);
		}

		static vector<governmentMapping> getGovernmentMappings()
		{
			return getInstance()->governmentMap;
		}

	private:
		static governmentMapper* instance;
		static governmentMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new governmentMapper();
			}
			return instance;
		}
		governmentMapper();
		void importGovernmentMappings(shared_ptr<Object> obj);
		void importPartyMappings(shared_ptr<Object> obj);

		string GetIdeologyForCountry(const V2Country* country, const string& Vic2RulingIdeology);
		string GetLeaderIdeologyForCountry(const V2Country* country, const string& Vic2RulingIdeology);
		string GetSupportedIdeology(const string& rulingIdeology, const string& Vic2Ideology);
		bool governmentMatches(const governmentMapping& mapping, const string& government);
		bool rulingIdeologyMatches(const governmentMapping& mapping, const string& rulingIdeology);

		vector<governmentMapping> governmentMap;
		vector<partyMapping> partyMap;
};



#endif // GOVERNMENTMAPPER_H