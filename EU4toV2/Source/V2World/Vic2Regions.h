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



#ifndef VIC2_REGIONS_H_
#define VIC2_REGIONS_H_



#include "newParser.h"
#include <map>
#include <set>
#include <string>



namespace Vic2
{
	class regions: commonItems::parser
	{
		public:
			static bool provinceIsInRegion(int province, const std::string& region)
			{
				return getInstance()->ProvinceIsInRegion(province, region);
			}

			static std::set<int> getProvincesInRegion(const std::string& region)
			{
				return getInstance()->GetProvincesInRegion(region);
			}

		private:
			static regions* instance;
			static regions* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new regions();
				}

				return instance;
			}
			regions();

			bool ProvinceIsInRegion(int province, const std::string& region);
			std::set<int> GetProvincesInRegion(const std::string& region);

			std::map<std::string, std::set<int>> theRegions;
	};
}



#endif // VIC2_REGIONS_H_