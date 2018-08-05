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



#ifndef GRAPHICS_MAPPER_H_
#define GRAPHICS_MAPPER_H_



#include "newParser.h"
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>



typedef std::map<std::string, std::vector<std::string>> cultureGroupToPortraitsMap;
typedef std::map<std::string, cultureGroupToPortraitsMap> ideologyToPortraitsMap;
typedef std::map<std::string, std::string> cultureGroupToGraphicalCultureMap;



class graphicsMapper: commonItems::parser
{
	public:
		graphicsMapper() noexcept;

		std::string getLeaderPortrait(const std::string& cultureGroup, const std::string& ideology);
		std::string getIdeologyMinisterPortrait(const std::string& cultureGroup, const std::string& ideology);
		std::string getGeneralPortrait(const std::string& cultureGroup) const;
		std::optional<std::string> getGraphicalCulture(const std::string& cultureGroup) const;
		std::optional<std::string> get2dGraphicalCulture(const std::string& cultureGroup) const;

	private:
		graphicsMapper(const graphicsMapper&) = delete;
		graphicsMapper& operator=(const graphicsMapper&) = delete;

		void loadLeaderPortraitMappings(const std::string& cultureGroup, std::map<std::string, std::vector<std::string>> portraitMappings);
		void loadIdeologyMinisterPortraitMappings(const std::string& cultureGroup, std::map<std::string, std::vector<std::string>> portraitMappings);

		std::optional<std::vector<std::string>> getLeaderPortraits(const std::string& cultureGroup, const std::string& ideology) const;
		std::optional<std::vector<std::string>> getIdeologyMinisterPortraits(const std::string& cultureGroup, const std::string& ideology) const;

		ideologyToPortraitsMap leaderPortraitMappings;
		ideologyToPortraitsMap ideologyMinisterMappings;
		cultureGroupToGraphicalCultureMap graphicalCultureMap;
		cultureGroupToGraphicalCultureMap graphicalCulture2dMap;

		std::mt19937 rng;
};




#endif //GRAPHICS_MAPPER_H_