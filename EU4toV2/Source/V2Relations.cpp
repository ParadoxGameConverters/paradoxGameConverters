#include "V2Relations.h"
#include "EU4Relations.h"



V2Relations::V2Relations(string newTag, EU4Relations* oldRelations)
{
	tag					= newTag;
	value					= oldRelations->getRelations();
	militaryAccess		= oldRelations->hasMilitaryAccess();
	lastSendDiplomat	= oldRelations->getDiplomatLastSent();
	lastWar				= oldRelations->getLastWar();
	level					= 2; // Neutral
}


void V2Relations::output(FILE* out) const
{
	fprintf(out, "\t%s=\n", tag.c_str());
	fprintf(out, "\t{\n");
	fprintf(out, "\t\tvalue=%d\n", value);
	if (militaryAccess)
	{
		fprintf(out, "\t\tmilitary_access=yes\n");
	}
	if (lastSendDiplomat.isSet())
	{
		fprintf(out, "\t\tlast_send_diplomat=\"%s\"\n", lastSendDiplomat.toString().c_str());
	}
	if (lastWar.isSet())
	{
		fprintf(out, "\t\tlast_war=\"%s\"\n", lastWar.toString().c_str());
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
	{
		return;
	}
	level = lvl;
}
