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



#ifndef CULTURE_GROUPS_H
#define CULTURE_GROUPS_H



#include "newParser.h"
#include <map>
#include <optional>
#include <string>
#include <vector>



namespace EU4
{
	class culture: commonItems::parser
	{
		public:
			culture(std::istream& theStream);

		private:
			std::string primaryTag;
			std::string graphicalCulture;
			std::vector<std::string> maleNames;
			std::vector<std::string> femaleNames;
			std::vector<std::string> dynastyNames;
	};


	class cultureGroup: commonItems::parser
	{
		public:
			cultureGroup(const std::string& name_, std::istream& theStream);

			std::string getName() const { return name; }
			std::map<std::string, culture> getCultures() const { return cultures; }

		private:
			std::string name;
			std::string graphicalCulture;
			std::vector<std::string> maleNames;
			std::vector<std::string> femaleNames;
			std::vector<std::string> dynastyNames;
			std::map<std::string, culture> cultures;
	};


	class cultureGroups: commonItems::parser
	{
		public:
			static std::optional<cultureGroup> getCulturalGroup(const std::string& culture)
			{
				return getInstance()->GetCulturalGroup(culture);
			}

			static std::map<std::string, cultureGroup> getCultureToGroupMap()
			{
				return getInstance()->cultureToGroupMap;
			}

			static std::vector<culture> getCulturesInGroup(const std::string& group)
			{
				return getInstance()->GetCulturesInGroup(group);
			}

		private:
			static cultureGroups* instance;
			static cultureGroups* getInstance()
			{
				if (instance == nullptr)
				{
					instance = new cultureGroups;
				}
				return instance;
			}

			cultureGroups();
			void addCulturesFromFile(const std::string& filename);

			std::optional<cultureGroup> GetCulturalGroup(const std::string& culture);
			std::vector<culture> GetCulturesInGroup(const std::string& group);

			std::map<std::string, std::vector<culture>> groupToCulturesMap;
			std::map<std::string, cultureGroup> cultureToGroupMap;
	};
}



#endif // CULTURE_GROUPS_H
