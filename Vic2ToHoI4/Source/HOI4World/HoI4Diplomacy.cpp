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



#include "HoI4Diplomacy.h"
#include "HoI4Agreement.h"
#include "Log.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"
#include <fstream>



void HoI4Diplomacy::output() const
{
	/*LOG(LogLevel::Debug) << "Writing diplomacy";
	fstream alliances("output/" + Configuration::getOutputName() + "/history/diplomacy/Alliances.txt");
	if (alliances.is_open())
	{
		LOG(LogLevel::Error) << "Could not create alliances history file";
		exit(-1);
	}

	fstream guarantees("output/" + Configuration::getOutputName() + "/history/diplomacy/Guarantees.txt");
	if (guarantees.is_open())
	{
		LOG(LogLevel::Error) << "Could not create guarantees history file";
		exit(-1);
	}

	fstream puppetStates("output/" + Configuration::getOutputName() + "/history/diplomacy/PuppetStates.txt");
	if (puppetStates.is_open())
	{
		LOG(LogLevel::Error) << "Could not create puppet states history file";
		exit(-1);
	}

	fstream relations("output/" + Configuration::getOutputName() + "/history/diplomacy/relations.txt");
	if (relations.is_open())
	{
		LOG(LogLevel::Error) << "Could not create relations history file";
		exit(-1);
	}
	
	for (auto itr: agreements)
	{
		if (itr->getType() == "guarantee")
		{
			guarantees << itr;
		}
		else if (itr->getType() == "vassal")
		{
			puppetStates << itr;
		}
		else if (itr->getType() == "alliance")
		{
			alliances << itr;
		}
		else if (itr->getType() == "relation")
		{
			relations << itr;
		}
		else
		{
			LOG(LogLevel::Warning) << "Cannot ouput diplomatic agreement type " << itr->getType();
			continue;
		}
	}
	
	alliances.close();
	guarantees.close();
	puppetStates.close();
	relations.close();*/
}


void HoI4Diplomacy::addAgreement(const HoI4Agreement* agr)
{
	bool alreadyExists = false;

	if (agr->getType() == "relation")
	{
		for (auto itr: agreements)
		{
			if (agr == itr)
			{
				alreadyExists = true;
				break;
			}
		}
	}

	if (!alreadyExists)
	{
		agreements.push_back(agr);
	}
}
