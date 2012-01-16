#include "HoI3Relations.h"


void HoI3Relations::init(string newTag)
{
	tag = newTag;
	value = 0;
	military_access = false;
}

void HoI3Relations::output(FILE* out)
{
	fprintf(out, "\t%s=\n", tag.c_str());
	fprintf(out, "\t{\n");
	fprintf(out, "\t\tvalue=%d\n", value);
	if (military_access)
	{
		fprintf(out, "\t\tmilitary_access=yes\n");
	}
	if (last_send_diplomat.isSet())
	{
		fprintf(out, "\t\tlast_send_diplomat=\"%s\"\n", last_send_diplomat.toString().c_str());
	}
	if (last_war.isSet())
	{
		fprintf(out, "\t\tlast_war=\"%s\"\n", last_war.toString().c_str());
	}
	fprintf(out, "\t}\n");
}
