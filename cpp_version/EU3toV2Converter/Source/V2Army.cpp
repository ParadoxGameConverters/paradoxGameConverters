#include "V2Army.h"
#include "Log.h"



static int nextArmyID = 0;


V2ArmyID::V2ArmyID()
{
	type = 40; // seems to be always 40, for army, navy, ship and regiment
	id = ++nextArmyID;
}


void V2ArmyID::output(FILE* out, int indentlevel)
{
	string indent(indentlevel, '\t');
	fprintf(out, "%sid=\n", indent.c_str());
	fprintf(out, "%s{\n", indent.c_str());
	fprintf(out, "%s\tid=%d\n", indent.c_str(), id);
	fprintf(out, "%s\ttype=%d\n", indent.c_str(), type);
	fprintf(out, "%s}\n", indent.c_str());
}


V2Regiment::V2Regiment(RegimentCategory rc) : category(rc)
{
	switch (rc)
	{
	case infantry:
		type = "infantry";
		isShip = false;
		break;
	case cavalry:
		type = "cavalry";
		isShip = false;
		break;
	case artillery:
		type = "artillery";
		isShip = false;
		break;
	case big_ship:
		type = "manowar";
		isShip = true;
		break;
	case galley:
	case light_ship:
		type = "frigate";
		isShip = true;
		break;
	case transport:
		type = "clipper_transport";
		isShip = true;
		break;
	default:
		log("Unexpected regiment category %d", rc);
		break;
	}
	setName("\"\""); // avoid crashes if no name set
}


void V2Regiment::output(FILE* out)
{
	if (isShip)
		fprintf(out, "\t\tship=\n");
	else
		fprintf(out, "\t\tregiment=\n");
	fprintf(out, "\t\t{\n");
	id.output(out, 3);
	fprintf(out, "\t\t\tname=\"%s\"\n", name.c_str());
	if (!isShip)
	{
		fprintf(out, "\t\t\tpop=\n");
		fprintf(out, "\t\t\t{\n");
		fprintf(out, "\t\t\t\tid=%d\n", popID);
		fprintf(out, "\t\t\t\ttype=46\n");
		fprintf(out, "\t\t\t}\n");
	}
	fprintf(out, "\t\t\tstrength=%f\n", strength);
	fprintf(out, "\t\t\ttype=%s\n", type.c_str());
	fprintf(out, "\t\t}\n");
}


V2Army::V2Army()
{
	leaderID = 0;
	for (int i = 0; i < num_reg_categories; ++i)
		army_remainders[i] = 0;
}


void V2Army::output(FILE* out)
{
	if (regiments.size() == 0)
	{
		log("	Army %s has no regiments after conversion; skipping.\n", name.c_str());
		return;
	}

	if (isNavy)
		fprintf(out, "\tnavy=\n");
	else
		fprintf(out, "\tarmy=\n");
	fprintf(out, "\t{\n");
	id.output(out, 2);
	fprintf(out, "\t\tname=\"%s\"\n", name.c_str());
	if (leaderID)
	{
		fprintf(out, "\t\tleader=\n");
		fprintf(out, "\t\t{\n");
		fprintf(out, "\t\t\tid=%d\n", leaderID);
		fprintf(out, "\t\t\ttype=37\n");
		fprintf(out, "\t\t}\n");
	}
	fprintf(out, "\t\tlocation=%d\n", location);
	fprintf(out, "\t\tsupplies=1.000\n");
	for (vector<V2Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		itr->output(out);
	}
	if (isNavy)
		fprintf(out, "\t\tat_sea=%d\n", at_sea);

	fprintf(out, "\t}\n");
}


void V2Army::addRegiment(V2Regiment reg)
{
	regiments.push_back(reg);
}


void V2Army::getRegimentCounts(int counts[num_reg_categories])
{
	for (vector<V2Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		counts[itr->getCategory()]++;
	}
}