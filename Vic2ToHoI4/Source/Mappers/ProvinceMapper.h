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



#ifndef PROVINCE_MAPPER_H
#define PROVINCE_MAPPER_H



#include "newParser.h"
#include <map>
#include <optional>
#include <vector>



typedef std::map<int, std::vector<int>> HoI4ToVic2ProvinceMapping;
typedef std::map<int, std::vector<int>> Vic2ToHoI4ProvinceMapping;



class provinceMapper: commonItems::parser
{
	public:
		provinceMapper() = default;
		void initialize();

		std::optional<std::vector<int>> getVic2ToHoI4ProvinceMapping(int Vic2Province) const;
		std::optional<std::vector<int>> getHoI4ToVic2ProvinceMapping(int HoI4Province) const;

	private:
		provinceMapper(const provinceMapper&) = delete;
		provinceMapper& operator=(const provinceMapper&) = delete;

		void checkAllHoI4ProvinesMapped() const;
		std::optional<int> getNextProvinceNumFromFile(std::ifstream& definitions) const;
		void verifyProvinceIsMapped(int provNum) const;

		HoI4ToVic2ProvinceMapping HoI4ToVic2ProvinceMap;
		Vic2ToHoI4ProvinceMapping Vic2ToHoI4ProvinceMap;
};


extern provinceMapper theProvinceMapper;



#endif // PROVINCE_MAPPER_H
