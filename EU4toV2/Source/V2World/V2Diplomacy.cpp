#include "V2Diplomacy.h"
#include "../log.h"
#include "../Configuration.h"



void V2Diplomacy::output() const
{
	FILE* alliances;
	if (fopen_s(&alliances, ("Output\\" + Configuration::getOutputName() + "\\history\\diplomacy\\Alliances.txt").c_str(), "w") != 0)
	{
		log("\tError: Could not create alliances history file");
		exit(-1);
	}

	FILE* guarantees;
	if (fopen_s(&guarantees, ("Output\\" + Configuration::getOutputName() + "\\history\\diplomacy\\Guarantees.txt").c_str(), "w") != 0)
	{
		log("\tError: Could not create guarantees history file");
		exit(-1);
	}

	FILE* puppetStates;
	if (fopen_s(&puppetStates, ("Output\\" + Configuration::getOutputName() + "\\history\\diplomacy\\PuppetStates.txt").c_str(), "w") != 0)
	{
		log("\tError: Could not create puppet states history file");
		exit(-1);
	}

	FILE* unions;
	if (fopen_s(&unions, ("Output\\" + Configuration::getOutputName() + "\\history\\diplomacy\\Unions.txt").c_str(), "w") != 0)
	{
		log("\tError: Could not create unions history file");
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
			log("\tWarning: Cannot ouput diplomatic agreement type %s\n", itr->type.c_str());
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