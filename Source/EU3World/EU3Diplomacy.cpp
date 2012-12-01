#include "EU3Diplomacy.h"
#include "..\Log.h"
#include "..\Parsers\Object.h"



EU3Agreement::EU3Agreement()
{
	type		= "";
	country1	= "";
	country2	= "";
}


void EU3Agreement::output(FILE* output)
{
	fprintf(output, "\t%s=\n", type.c_str());
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tfirst=\"%s\"\n", country1.c_str());
	fprintf(output, "\t\tsecond=\"%s\"\n", country2.c_str());
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
	for (vector<EU3Agreement>::iterator agreementItr = agreements.begin(); agreementItr != agreements.end(); agreementItr++)
	{
		agreementItr->output(output);
	}
	fprintf(output, "}\n");
}