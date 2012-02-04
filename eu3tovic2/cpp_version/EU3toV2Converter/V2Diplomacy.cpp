#include "V2Diplomacy.h"

void V2Diplomacy::output(FILE * out)
{
	fprintf(out, "diplomacy=\n");
	fprintf(out, "{\n");
	for (vector<V2Agreement>::iterator itr = agreements.begin(); itr != agreements.end(); ++itr)
	{
		fprintf(out, "\t%s=\n", itr->type.c_str());
		fprintf(out, "\t{\n");
		fprintf(out, "\t\tfirst=\"%s\"\n", itr->country1.c_str());
		fprintf(out, "\t\tsecond=\"%s\"\n", itr->country2.c_str());
		fprintf(out, "\t\tstart_date=\"%s\"\n", itr->start_date.toString().c_str());
		fprintf(out, "\t}\n");
	}
	fprintf(out, "}\n");
}