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



#ifndef EU4_CONTINENTS_H
#define EU4_CONTINENTS_H



#include "newParser.h"
#include <map>
#include <string>



namespace EU4
{
	class continents: commonItems::parser
	{
		public:
			static std::optional<std::string> getEU4Continent(int EU4Province)
			{
				return getInstance()->GetEU4Continent(EU4Province);
			}

		private:
			static continents* instance;
			static continents* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new continents;
				}

				return instance;
			}

			continents();
			void initContinentMap(const std::string& filename);

			std::optional<std::string> GetEU4Continent(int EU4Province);


			std::map<int, std::string> continentMap;
	};
}



#endif // EU4_CONTINENTS_H
