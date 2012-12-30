#include "EU3Diplomacy.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"
#include "EU3Country.h"



EU3Agreement::EU3Agreement()
{
	type		= "";
	country1	= NULL;
	country2	= NULL;
}


void EU3Agreement::output(FILE* output)
{
	fprintf(output, "\t%s=\n", type.c_str());
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tfirst=\"%s\"\n", country1->getTag().c_str());
	fprintf(output, "\t\tsecond=\"%s\"\n", country2->getTag().c_str());
	fprintf(output, "\t\tstart_date=\"%s\"\n", startDate.toString().c_str());
	fprintf(output, "\t}\n");
}


EU3Diplomacy::EU3Diplomacy()
{
	agreements.clear();
}


void EU3Diplomacy::output(FILE* output)
{
	fprintf(output, "diplomacy=\n");
	fprintf(output, "{\n");
	for (vector<EU3Agreement*>::iterator agreementItr = agreements.begin(); agreementItr != agreements.end(); agreementItr++)
	{
		(*agreementItr)->output(output);
	}
	fprintf(output, "}\n");
}


void EU3Diplomacy::removeAgreement(EU3Agreement* agr)
{
	for (vector<EU3Agreement*>::iterator agreementItr = agreements.begin(); agreementItr != agreements.end(); agreementItr++)
	{
		if (*agreementItr == agr)
		{
			agreements.erase(agreementItr);
			break;
		}
	}
}