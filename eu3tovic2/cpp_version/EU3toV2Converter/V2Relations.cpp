#include "V2Relations.h"


void V2Relations::init(string newTag)
{
	tag = newTag;
	value = 0;
	level = 2; // Neutral
	military_access = false;
}

void V2Relations::output(FILE* out)
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
	if (level != 2)
	{
		fprintf(out, "\t\tlevel=%d\n", level);
	}
	fprintf(out, "\t}\n");
}

void V2Relations::setLevel(int lvl)
{
	if (lvl < 0 || lvl > 5)
		return;
	level = lvl;
}
