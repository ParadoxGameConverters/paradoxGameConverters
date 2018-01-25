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



#ifndef PROVINCE_MAPPER_H
#define PROVINCE_MAPPER_H



#include <map>
#include <memory>
#include <unordered_set>
#include <vector>
using namespace std;


class EU4Version;
class Object;



class provinceMapper
{
	public:
		static const vector<int> getVic2ProvinceNumbers(int EU4ProvinceNumber)
		{
			return getInstance()->GetVic2ProvinceNumbers(EU4ProvinceNumber);
		}

		static const vector<int> getEU4ProvinceNumbers(int Vic2ProvinceNumber)
		{
			return getInstance()->GetEU4ProvinceNumbers(Vic2ProvinceNumber);
		}

		static bool isProvinceResettable(int Vic2ProvinceNumber)
		{
			return getInstance()->IsProvinceResettable(Vic2ProvinceNumber);
		}

	private:
		static provinceMapper* instance;
		static provinceMapper* getInstance()
		{
			if (instance == NULL)
			{
				instance = new provinceMapper;
			}

			return instance;
		}

		provinceMapper();
		void initProvinceMap(shared_ptr<Object> obj);
		int getMappingsIndex(vector<shared_ptr<Object>> versions);
		void createMappings(shared_ptr<Object> mapping);

		const vector<int> GetVic2ProvinceNumbers(int EU4ProvinceNumber);
		const vector<int> GetEU4ProvinceNumbers(int Vic2ProvinceNumber);
		bool IsProvinceResettable(int Vic2ProvinceNumber);

		map<int, vector<int>> Vic2ToEU4ProvinceMap;
		map<int, vector<int>> EU4ToVic2ProvinceMap;
		unordered_set<int> resettableProvinces;
};



#endif // PROVINCE_MAPPER_H