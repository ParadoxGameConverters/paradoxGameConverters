/*Copyright (c) 2018 The Paradox Game Converters Project

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



#ifndef EU4_REGIONS_H
#define EU4_REGIONS_H



#include "newParser.h"
#include "Region.h"
#include <map>
#include <memory>
#include <set>
#include <string>



class Object;
namespace EU4
{
	class areas;

	class Regions: commonItems::parser
	{
		public:
			static bool provinceInRegion(int province, const std::string& regionName)
			{
				return getInstance()->ProvinceInRegion(province, regionName);
			}

		private:
			static Regions* instance;
			static Regions* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new Regions;
				}
				return instance;
			}

			Regions();

			void initEU4RegionsOldVersion();

			void initEU4RegionsNewVersion();
			void initEU4RegionsFile(const EU4::areas& areas, const std::string& regionsFilename);

			bool ProvinceInRegion(int province, const std::string& regionName);

			std::map<int, std::set<std::string>> EU4RegionsMap;
			std::map<std::string, EU4::region> regions;
	};
}



#endif //EU4_REGIONS_H
