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



#ifndef CK2_TITLE_MAPPER_H
#define CK2_TITLE_MAPPER_H



#include <map>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "newParser.h"



namespace mappers
{
	class CK2TitleMapper: commonItems::parser
	{
		public:
			static const std::optional<std::string> getTitle(std::string name)
			{
				return getInstance()->GetTitle(name);
			}

			static bool doesTitleExist(std::string title)
			{
				return getInstance()->DoesTitleExist(title);
			}

			static const std::optional<std::string> getRandomIslamicFlag()
			{
				return getInstance()->GetRandomIslamicFlag();
			}

			static const std::optional<std::string> getRandomIndianFlag()
			{
				return getInstance()->GetRandomIndianFlag();
			}

		private:
			static CK2TitleMapper* instance;
			static CK2TitleMapper* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new CK2TitleMapper;
				}
				return instance;
			}

			CK2TitleMapper();

			std::optional<std::string> GetTitle(std::string name);
			bool DoesTitleExist(std::string title);
			std::optional<std::string> GetRandomIslamicFlag();
			std::optional<std::string> GetRandomIndianFlag();

			std::map<std::string, std::string> titleMap; // <name, title>
			std::set<std::string> titles;
			std::vector<std::string> islamicFlags;
			std::vector<std::string> indianFlags;
			std::mt19937 generator;
	};
}



#endif // CK2_TITLE_MAPPER_H