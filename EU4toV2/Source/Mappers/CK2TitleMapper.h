/*Copyright (c) 2016 The Paradox Game Converters Project

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



#ifndef CK2_TITLE_MAPPER_H
#define CK2_TITLE_MAPPER_H



#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>
using namespace std;



class Object;



class CK2TitleMapper
{
	public:
		static const string getTitle(string name)
		{
			return getInstance()->GetTitle(name);
		}

		static bool doesTitleExist(string title)
		{
			return getInstance()->DoesTitleExist(title);
		}

		static const string getRandomIslamicFlag()
		{
			return getInstance()->GetRandomIslamicFlag();
		}

		static const string getRandomIndianFlag()
		{
			return getInstance()->GetRandomIndianFlag();
		}

	private:
		static CK2TitleMapper* instance;
		static CK2TitleMapper* getInstance()
		{
			if (instance == NULL)
			{
				instance = new CK2TitleMapper;
			}
			return instance;
		}

		CK2TitleMapper();
		void initCK2TitleMap(Object* obj);

		string GetTitle(string name);
		bool DoesTitleExist(string title);
		string GetRandomIslamicFlag();
		string GetRandomIndianFlag();

		map<string, string> titleMap; // <name, title>
		set<string> titles;
		vector<string> islamicFlags;
		vector<string> indianFlags;
		mt19937 generator;
};



#endif // CK2_TITLE_MAPPER_H