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



#ifndef EU4_CULTURE_GROUP_MAPPER_H
#define EU4_CULTURE_GROUP_MAPPER_H



#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;



class Object;



class EU4CultureGroupMapper
{
	public:
		static string getCulturalGroup(const string& culture)
		{
			return getInstance()->GetCulturalGroup(culture);
		}

		static map<string, string> getCultureToGroupMap()
		{
			return getInstance()->cultureToGroupMap;
		}

		static vector<string> getCulturesInGroup(const string& group)
		{
			return getInstance()->GetCulturesInGroup(group);
		}

	private:
		static EU4CultureGroupMapper* instance;
		static EU4CultureGroupMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new EU4CultureGroupMapper;
			}
			return instance;
		}

		EU4CultureGroupMapper();
		void addCulturesFromFile(const string& filename);
		void initCultureGroups(shared_ptr<Object> obj);
		bool isIgnorableItem(const string& key);

		string GetCulturalGroup(const string& culture);
		vector<string> GetCulturesInGroup(const string& group);

		map<string, vector<string>> groupToCulturesMap;
		map<string, string> cultureToGroupMap;
};



#endif // EU4_CULTURE_GROUP_MAPPER_H
