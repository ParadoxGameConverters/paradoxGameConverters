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



#include "V2Diplomacy.h"
#include "Log.h"
#include "../Configuration.h"
#include "OSCompatibilityLayer.h"



void V2Diplomacy::output() const
{
	LOG(LogLevel::Debug) << "Writing diplomacy";

	FILE* alliances;
	if (fopen_s(&alliances, ("Output/" + Configuration::getOutputName() + "/history/diplomacy/Alliances.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create alliances history file";
		exit(-1);
	}

	FILE* guarantees;
	if (fopen_s(&guarantees, ("Output/" + Configuration::getOutputName() + "/history/diplomacy/Guarantees.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create guarantees history file";
		exit(-1);
	}

	FILE* puppetStates;
	if (fopen_s(&puppetStates, ("Output/" + Configuration::getOutputName() + "/history/diplomacy/PuppetStates.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create puppet states history file";
		exit(-1);
	}

	FILE* unions;
	if (fopen_s(&unions, ("Output/" + Configuration::getOutputName() + "/history/diplomacy/Unions.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create unions history file";
		exit(-1);
	}
	
	FILE* out;
	for (vector<V2Agreement>::const_iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		if (itr->type == "guarantee")
		{
			out = guarantees;
		}
		else if (itr->type == "union")
		{
			out = unions;
		}
		else if (itr->type == "vassal")
		{
			out = puppetStates;
		}
		else if (itr->type == "alliance")
		{
			out = alliances;
		}
		else
		{
			LOG(LogLevel::Warning) << "Cannot ouput diplomatic agreement type " << itr->type;
			continue;
		}
		fprintf(out, "%s=\n", itr->type.c_str());
		fprintf(out, "{\n");
		fprintf(out, "\tfirst=\"%s\"\n", itr->country1.c_str());
		fprintf(out, "\tsecond=\"%s\"\n", itr->country2.c_str());
		fprintf(out, "\tstart_date=\"%s\"\n", itr->start_date.toString().c_str());
		fprintf(out, "\tend_date=\"1936.1.1\"\n");
		fprintf(out, "}\n");
		fprintf(out, "\n");
	}
	
	fclose(alliances);
	fclose(guarantees);
	fclose(puppetStates);
	fclose(unions);
}