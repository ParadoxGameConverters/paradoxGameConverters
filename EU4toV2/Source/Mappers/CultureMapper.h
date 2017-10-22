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



#ifndef CULTURE_MAPPER_H
#define CULTURE_MAPPER_H



#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;



class Object;



typedef struct
{
	string srcCulture;
	string dstCulture;
	map<string, string> distinguishers;	// type, details
} cultureStruct;



class cultureMapper
{
	public:
		static bool cultureMatch(const string& srcCulture, string& dstCulture, const string& religion = "", int EU4Province = -1, const string& ownerTag = "")
		{
			return getInstance()->CultureMatch(srcCulture, dstCulture, religion, EU4Province, ownerTag);
		}

		static bool slaveCultureMatch(const string& srcCulture, string& dstCulture, const string& religion = "", int EU4Province = -1, const string& ownerTag = "")
		{
			return getInstance()->SlaveCultureMatch(srcCulture, dstCulture, religion, EU4Province, ownerTag);
		}

	private:
		static cultureMapper* instance;
		static cultureMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new cultureMapper;
			}
			return instance;
		}

		cultureMapper();
		void initCultureMap(shared_ptr<Object> cultureMapObj, shared_ptr<Object> slaveCultureMapObj);
		vector<cultureStruct> createNewRules(shared_ptr<Object> ruleObj);

		bool CultureMatch(const string& srcCulture, string& dstCulture, const string& religion = "", int EU4Province = -1, const string& ownerTag = "");
		bool SlaveCultureMatch(const string& srcCulture, string& dstCulture, const string& religion = "", int EU4Province = -1, const string& ownerTag = "");
		bool distinguishersMatch(const map<string, string>& distinguishers, const string& religion = "", int EU4Province = -1, const string& ownerTag = "");

		vector<cultureStruct> cultureMap;
		vector<cultureStruct> slaveCultureMap;
};



#endif // CULTURE_MAPPER_H