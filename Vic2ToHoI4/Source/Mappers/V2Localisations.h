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



#ifndef V2LOCALISATIONS_H_
#define V2LOCALISATIONS_H_



#include <map>
#include <string>
#include <unordered_map>
using namespace std;



class V2Localisations
{
	public:
		static const string GetTextInLanguage(const string& key, const string& language)
		{
			return getInstance()->ActuallyGetTextInLanguage(key, language);
		}
		static const map<string, string>& GetTextInEachLanguage(const string& key)
		{
			return getInstance()->ActuallyGetTextInEachLanguage(key);
		}
		static void UpdateDomainCountry(const string& tag, const string& dominionName)
		{
			return getInstance()->ActuallyUpdateDomainCountry(tag, dominionName);
		}

	private:
		static V2Localisations* instance;
		static V2Localisations* getInstance()
		{
			if (instance == NULL)
			{
				instance = new V2Localisations();
			}
			return instance;
		}

		V2Localisations();
		void ReadFromAllFilesInFolder(const string& folderPath);
		void ReadFromFile(const string& fileName);
		void processLine(string line);
		string getNextLocalisation(string line, int& division);
		string replaceBadCharacters(string localisation);

		const string ActuallyGetTextInLanguage(const string& key, const string& language) const;
		const map<string, string>& ActuallyGetTextInEachLanguage(const string& key) const;
		void ActuallyUpdateDomainCountry(const string & tag, const string & dominionName);

		typedef map<string, string> LanguageToLocalisationMap;
		typedef unordered_map<string, LanguageToLocalisationMap> KeyToLocalisationsMap;
		KeyToLocalisationsMap localisations;
};



#endif	// V2LOCALISATIONS_H_