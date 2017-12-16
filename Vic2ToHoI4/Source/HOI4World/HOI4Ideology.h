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



#ifndef HOI4_IDEOLOGY
#define HOI4_IDEOLOGY



#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include "../Color.h"
using namespace std;



class Object;



class HoI4Ideology
{
	public:
		explicit HoI4Ideology(shared_ptr<Object> obj);
		void output(ofstream& file) const;

		vector<string> getTypes() const { return types; }

	private:
		HoI4Ideology(const HoI4Ideology&) = delete;
		HoI4Ideology& operator=(const HoI4Ideology&) = delete;

		void outputTypes(ofstream& file) const;
		void outputDynamicFactionNames(ofstream& file) const;
		void outputTheColor(ofstream& file) const;
		void outputRules(ofstream& file) const;
		void outputOnWorldTension(ofstream& file) const;
		void outputModifiers(ofstream& file) const;
		void outputFactionModifiers(ofstream& file) const;
		void outputCans(ofstream& file) const;
		void outputAI(ofstream& file) const;

		string ideologyName;
		vector<string> types;
		vector<string> dynamicFactionNames;
		ConverterColor::Color* theColor;
		map<string, string> rules;
		float warImpactOnWorldTension;
		float factionImpactOnWorldTension;
		map<string, float> modifiers;
		map<string, float> factionModifiers;
		map<string, string> cans;
		string AI;
};



#endif // HOI4_IDEOLOGY