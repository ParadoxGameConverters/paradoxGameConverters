#include "HoI3Army.h"
#include "Log.h"
#include "Configuration.h"
#include "Parsers/Parser.h"


static int nextArmyID = 0;

HoI3ArmyID::HoI3ArmyID()
{
	type = 41; // seems to be always 41, for army, navy, ship and regiment
	id = ++nextArmyID;
}

void HoI3ArmyID::output(FILE* out, int indentlevel)
{
	string indent(indentlevel, '\t');
	fprintf(out, "%sid=\n", indent.c_str());
	fprintf(out, "%s{\n", indent.c_str());
	fprintf(out, "%s\tid=%d\n", indent.c_str(), id);
	fprintf(out, "%s\ttype=%d\n", indent.c_str(), type);
	fprintf(out, "%s}\n", indent.c_str());
}

HoI3RegimentType::HoI3RegimentType(string type)
{
	string filename = Configuration::getHoI3Path() + "\\units\\" + type + ".txt";
	Object* obj = doParseFile(filename.c_str());
	obj = obj->getLeaves()[0];
	name = obj->getKey();

	string unit_type = obj->getLeaf("type");
	if (unit_type == "air")
		force_type = air;
	else if (unit_type == "land")
		force_type = land;
	else if (unit_type == "naval")
		force_type = navy;
	else
		log("Error: Possible bad unit type in %s!\n", filename.c_str());

	string max_str = obj->getLeaf("max_strength");
	max_strength = atoi(max_str.c_str());
}

void HoI3Regiment::output(FILE* out)
{
	switch (type.getForceType())
	{
	case land:
		fprintf(out, "\t\tregiment=\n");
		break;
	case navy:
		fprintf(out, "\t\tship=\n");
		break;
	case air:
		fprintf(out, "\t\twing=\n");
		break;
	}
	fprintf(out, "\t\t{\n");
	id.output(out, 3);
	fprintf(out, "\t\t\tname=\"%s\"\n", name.c_str());
	fprintf(out, "\t\t\ttype=\"%s\"\n", type.getName().c_str());
	fprintf(out, "\t\t\torganisation=%f\n", organization);
	fprintf(out, "\t\t\tstrength=%f\n", strength);
	//XXX: what does "highest=%f" do??
	if (type.getForceType() == land)
		fprintf(out, "\t\t\tis_reserve=no\n");
	//XXX: upgrade levels go here
	fprintf(out, "\t\t}\n");
}

HoI3Army::HoI3Army()
{
	leaderID = 0;
}

void HoI3Army::output(FILE* out)
{
	if (regiments.size() == 0)
	{
		log("Division %s has no regiments after conversion; skipping.\n", name.c_str());
		return;
	}

	switch (force_type)
	{
	case land:
		fprintf(out, "\tdivision=\n");
		break;
	case navy:
		fprintf(out, "\tnavy=\n");
		break;
	case air:
		fprintf(out, "\tair=\n");
		break;
	}
	fprintf(out, "\t{\n");
	id.output(out, 2);
	fprintf(out, "\t\tname=\"%s\"\n", name.c_str());
	if (force_type == air) // probably should assign bases for navies too, but they might be at sea
		fprintf(out, "\t\tbase=%d\n", location);
	if (leaderID)
	{
		fprintf(out, "\t\tleader=\n");
		fprintf(out, "\t\t{\n");
		fprintf(out, "\t\t\tid=%d\n", leaderID);
		fprintf(out, "\t\t\ttype=37\n");
		fprintf(out, "\t\t}\n");
	}
	fprintf(out, "\t\tlocation=%d\n", location);
	fprintf(out, "\t\tis_prioritized=no\n");
	fprintf(out, "\t\tcan_reinforce=yes\n");
	fprintf(out, "\t\tcan_upgrade=yes\n");
	fprintf(out, "\t\tsupplies=%f\n", supplies);
	fprintf(out, "\t\tfuel=%f\n", fuel);
	for (vector<HoI3Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		itr->output(out);
	}
	if (force_type == navy)
		fprintf(out, "\t\tat_sea=%d\n", at_sea);

	fprintf(out, "\t}\n");
}

void HoI3Army::addRegiment(HoI3Regiment reg)
{
	// only add ships to navies and regiments to armies
	if (force_type != reg.getForceType())
	{
		log("Error: attempted to add unit to parent of incorrect type!\n");
		return;
	}
	regiments.push_back(reg);
}
