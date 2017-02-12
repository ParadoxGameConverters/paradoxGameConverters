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



#ifndef PORTRAITMAPPER_H
#define PORTRAITMAPPER_H



#include <map>
#include <random>
#include <string>
#include <vector>
using namespace std;



typedef struct
{
	map<string, vector<string>> element;
} cultureGroupToPortraitsMap;

typedef struct 
{
	map<string, cultureGroupToPortraitsMap> element;
} ideologyToPortraitsMap;



class portraitMapper
{
	public:
		static string getPortrait(string cultureGroup, string ideology)
		{
			return getInstance()->GetPortrait(cultureGroup, ideology);
		}

		static vector<string> getPortraits(string cultureGroup, string ideology)
		{
			return getInstance()->GetPortraits(cultureGroup, ideology);
		}
	private:
		static portraitMapper* instance;
		static portraitMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new portraitMapper;
			}
			return instance;
		}
		portraitMapper();

		string GetPortrait(string cultureGroup, string ideology);
		vector<string> GetPortraits(string cultureGroup, string ideology);

		ideologyToPortraitsMap mappings;

		std::mt19937 rng;
};




#endif //PORTRAITMAPPER_H