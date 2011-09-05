#include "V2Leader.h"
#include "V2Country.h"

static int nextId = 0;

void V2Leader::init(V2Country* _country)
{
	id = ++nextId;

	country = _country->getTag();
	// TODO: figure out picture
}

void V2Leader::output(FILE *output)
{
	fprintf(output, "\tleader=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\tname=\"%s\"\n", name.c_str());
	fprintf(output, "\t\tdate=\"%s\"\n", activationDate.toString().c_str());
	if (isLand)
		fprintf(output, "\t\ttype=land\n");
	else
		fprintf(output, "\t\ttype=sea\n");
	fprintf(output, "\t\tpersonality=\"%s\"\n", personality.c_str());
	fprintf(output, "\t\tbackground=\"%s\"\n", background.c_str());
	fprintf(output, "\t\tcountry=\"%s\"\n", country.c_str());
	//fprintf(output, "\t\tpicture=\"%s\"\n", picture.c_str());
	fprintf(output, "\t\tid=\n", country.c_str());
	fprintf(output, "\t\t{\n", country.c_str());
	fprintf(output, "\t\t\tid=%d\n", id);
	fprintf(output, "\t\t\ttype=37\n");
	fprintf(output, "\t\t}\n", country.c_str());
	fprintf(output, "\t}\n");
}
