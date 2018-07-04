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



#ifndef HOI4_IDEOLOGY
#define HOI4_IDEOLOGY



#include "newParser.h"
#include "../Color.h"
#include <fstream>
#include <map>
#include <string>
#include <vector>



class HoI4Ideology: commonItems::parser
{
	public:
		explicit HoI4Ideology(const std::string& ideologyName, std::istream& theStream);
		void output(std::ofstream& file) const;

		std::vector<std::string> getTypes() const { return types; }

	private:
		HoI4Ideology(const HoI4Ideology&) = delete;
		HoI4Ideology& operator=(const HoI4Ideology&) = delete;

		void outputTypes(std::ofstream& file) const;
		void outputDynamicFactionNames(std::ofstream& file) const;
		void outputTheColor(std::ofstream& file) const;
		void outputRules(std::ofstream& file) const;
		void outputOnWorldTension(std::ofstream& file) const;
		void outputModifiers(std::ofstream& file) const;
		void outputFactionModifiers(std::ofstream& file) const;
		void outputCans(std::ofstream& file) const;
		void outputAI(std::ofstream& file) const;

		std::string ideologyName;
		std::vector<std::string> types;
		std::vector<std::string> dynamicFactionNames;
		ConverterColor::Color* theColor = nullptr;
		std::map<std::string, std::string> rules;
		float warImpactOnWorldTension = 0.0;
		float factionImpactOnWorldTension = 0.0;
		std::map<std::string, float> modifiers;
		std::map<std::string, float> factionModifiers;
		std::map<std::string, std::string> cans;
		std::string AI;
};



#endif // HOI4_IDEOLOGY