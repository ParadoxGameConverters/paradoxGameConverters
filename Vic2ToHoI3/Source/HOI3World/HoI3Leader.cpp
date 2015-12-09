/*Copyright (c) 2015 The Paradox Game Converters Project

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


#include "HoI3Leader.h"
#include "..\Configuration.h"



HoI3Leader::HoI3Leader(vector<string>& firstNames, vector<string>& lastNames, string _country, string _type, leaderTraitsMap& _traitsMap, vector<string>& portraits)
{
	ID			= Configuration::getNextLeaderID();
	name		= firstNames[rand() % firstNames.size()] + " " + lastNames[rand() % lastNames.size()];
	country	= _country;
	type		= _type;
	loyalty	= 1.0f;

	picture	= portraits[rand() % portraits.size()];

	if (rand() % 4 > 0)
	{
		vector<string> allTraits = _traitsMap.find(_type)->second;
		traits.push_back(allTraits[rand() % allTraits.size()]);

		if (rand() % 10 > 8)
		{
			string secondTrait = allTraits[rand() % allTraits.size()];
			while (traits[0] == secondTrait)
			{
				secondTrait = allTraits[rand() % allTraits.size()];
			}
			traits.push_back(secondTrait);
		}
	}
}


void HoI3Leader::output(FILE * output)
{
	fprintf(output, "%d = {\n", ID);
	fprintf(output, "\tname = \"%s\"\n", name.c_str());
	fprintf(output, "\tcountry = %s\n", country.c_str());
	fprintf(output, "\ttype = %s\n", type.c_str());
	fprintf(output, "\tskill = 0\n");
	fprintf(output, "\tmax_skill = 9\n");
	fprintf(output, "\tloyalty = %.2f\n", loyalty);
	fprintf(output, "\tpicture = %s\n", picture.c_str());
	for (auto trait: traits)
	{
		fprintf(output, "\tadd_trait = %s\n", trait.c_str());
	}
	fprintf(output, "\thistory = {\n");
	fprintf(output, "\t\t1936.1.1 = { rank = 1 }\n");
	fprintf(output, "\t\t1970.1.1 = { rank = 0 }\n");
	fprintf(output, "\t}\n");
	fprintf(output, "}\n");
}