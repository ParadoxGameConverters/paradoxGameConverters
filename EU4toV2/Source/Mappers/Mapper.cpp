/*Copyright (c) 2014 The Paradox Game Converters Project

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



#include "Mapper.h"
#include "Object.h"
#include "../V2World/V2Localisation.h"



colonyFlagset initColonyFlagset(Object* obj)
{
	colonyFlagset colonyMap;									// the culture mapping
	vector<Object*> colonialRules = obj->getLeaves();			// the culture mapping rules en masse
	vector<Object*> regions = colonialRules[1]->getLeaves();	// the individual colonial flags

	for (vector<Object*>::iterator i = regions.begin(); i != regions.end(); i++)
	{
		std::string region = (*i)->getKey();
		vector<Object*> flags = (*i)->getLeaves();	// the flags in this region
		for (vector<Object*>::iterator j = flags.begin(); j != flags.end(); j++)
		{
			vector<Object*>			items = (*j)->getLeaves();	// the items in this rule
			
			shared_ptr<colonyFlag> flag(new colonyFlag());
			flag->region = region;
			flag->unique = false;
			flag->overlord = "";
			vector<Object*>::iterator k = items.begin();
			flag->name = (*k)->getLeaf();
			std::transform(flag->name.begin(), flag->name.end(), flag->name.begin(), ::tolower);

			for (; k != items.end(); k++)
			{
				if ((*k)->getKey() == "name")
				{
					string name = (*k)->getLeaf();
					name = V2Localisation::Convert(name);
					std::transform(name.begin(), name.end(), name.begin(), ::tolower);

					colonyMap[name] = flag;
				}
				if ((*k)->getKey() == "unique")
				{
					flag->unique = true;
				}

			}
		}
	}

	return colonyMap;
}


string CardinalToOrdinal(int cardinal)
{
	int hundredRem = cardinal % 100;
	int tenRem = cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}