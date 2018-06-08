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



#ifndef VIC2_STATE_DEFINITIONS_H
#define VIC2_STATE_DEFINITIONS_H



#include "newParser.h"
#include <map>
#include <memory>
#include <optional>
#include <set>



namespace Vic2
{

class StateDefinitions: commonItems::parser
{
	public:
		StateDefinitions() = default;

		void initialize();

		std::set<int> getAllProvinces(int provinceNumber) const;
		std::optional<std::string> getStateID(int provinceNumber) const;
		std::optional<int> getCapitalProvince(const std::string& stateID) const;

	private:
		StateDefinitions(const StateDefinitions&) = delete;
		StateDefinitions& operator=(const StateDefinitions&) = delete;

		std::map<int, std::set<int>> stateMap; // < province, all other provinces in state >
		std::map<int, std::string> provinceToIDMap;
		std::map<std::string, int> stateToCapitalMap;
};


extern StateDefinitions theStateDefinitions;

}



#endif // VIC2_STATE_DEFINITIONS_H

