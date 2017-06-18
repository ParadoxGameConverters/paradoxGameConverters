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



#ifndef EU4REGION_MAPPER_H
#define EU4REGION_MAPPER_H



#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;



class Object;



class EU4RegionMapper
{
	public:
		static bool provinceInRegion(int province, const string& region)
		{
			return getInstance()->ProvinceInRegion(province, region);
		}

	private:
		static EU4RegionMapper* instance;
		static EU4RegionMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new EU4RegionMapper;
			}
			return instance;
		}

		EU4RegionMapper();

		void attemptOldVersion();
		void initEU4RegionMapOldVersion(Object *obj);
		void insertMapping(int provinceNumber, string regionName);

		void doNewVersion();
		void makeWorkingAreaTxt(const string& path);
		void initEU4RegionMap(Object* areasObj, Object* regionsObj);
		map<string, vector<int>> getAreaToProvincesMapping(Object* areasObj);

		bool ProvinceInRegion(int province, const string& region);
		set<string> getRegions(int province);

		map<int, set<string>> EU4RegionsMap;
};



#endif //EU4REGION_MAPPER_H
