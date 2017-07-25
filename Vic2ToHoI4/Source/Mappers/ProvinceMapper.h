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



#ifndef PROVINCE_MAPPING_H
#define PROVINCE_MAPPING_H



#include <map>
#include <memory>
#include <unordered_set>
#include <vector>
using namespace std;



class Object;



typedef map< int, vector<int> > HoI4ToVic2ProvinceMapping;
typedef map< int, vector<int> > Vic2ToHoI4ProvinceMapping;



class provinceMapper
{
	public:
		static const Vic2ToHoI4ProvinceMapping& getVic2ToHoI4ProvinceMapping()
		{
			return getInstance()->Vic2ToHoI4ProvinceMap;
		}

		static const HoI4ToVic2ProvinceMapping& getHoI4ToVic2ProvinceMapping()
		{
			return getInstance()->HoI4ToVic2ProvinceMap;
		}

	private:
		static provinceMapper* instance;
		static provinceMapper* getInstance()
		{
			if (instance == NULL)
			{
				instance = new provinceMapper();
			}

			return instance;
		}
		provinceMapper();


		void initProvinceMap(shared_ptr<Object> parsedMappingsFile);
		vector<shared_ptr<Object>> getCorrectMappingVersion(const vector<shared_ptr<Object>>& versions);
		void processMappings(const vector<shared_ptr<Object>>& mappings);
		vector<int> getHoI4ProvinceNums(int v2ProvinceNum);
		void insertIntoHoI4ToVic2ProvinceMap(const vector<int>& Vic2Nums, const vector<int>& HoI4nums);
		void insertIntoVic2ToHoI4ProvinceMap(const vector<int>& Vic2Nums, const vector<int>& HoI4nums);

		void checkAllHoI4ProvinesMapped();
		int getNextProvinceNumFromFile(ifstream& definitions);
		void verifyProvinceIsMapped(int provNum);

		HoI4ToVic2ProvinceMapping HoI4ToVic2ProvinceMap;
		Vic2ToHoI4ProvinceMapping Vic2ToHoI4ProvinceMap;
};



#endif // PROVINCE_MAPPING_H
