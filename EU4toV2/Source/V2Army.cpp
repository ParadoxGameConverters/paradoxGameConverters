#include "V2Army.h"
#include "Log.h"



static int nextArmyID = 0;


V2ArmyID::V2ArmyID()
{
	type = 40; // seems to be always 40, for army, navy, ship and regiment
	id = ++nextArmyID;
}


void V2ArmyID::output(FILE* out, int indentlevel) const
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
	name		= "\"\"";
	popID		= 0;
	strength	= 0.0;
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
		case heavy_ship:
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
}


void V2Regiment::output(FILE* out) const
{
	if (isShip)
	{
		fprintf(out, "\t\tship=\n");
	}
	else
	{
		fprintf(out, "\t\tregiment=\n");
	}
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


V2Army::V2Army(EU4Army* oldArmy, map<int, int> leaderIDMap)
{
	name			= oldArmy->getName();
	location		= -1;
	regiments.clear();
	memset(armyRemainders, 0, sizeof(armyRemainders));
	sourceArmy	= oldArmy;
	at_sea		= oldArmy->getAtSea();

	map<int, int>::const_iterator lmapitr = leaderIDMap.find( oldArmy->getLeaderID() );
	if (lmapitr != leaderIDMap.end())
	{
			leaderID = lmapitr->second;
	}
	else
	{
		leaderID = 0;
	}

	isNavy		= false;
}


void V2Army::output(FILE* out) const
{
	if (regiments.size() == 0)
	{
		log("	Army %s has no regiments after conversion; skipping.\n", name.c_str());
		return;
	}
	if (isNavy)
	{
		fprintf(out, "\tnavy=\n");
	}
	else
	{
		fprintf(out, "\tarmy=\n");
	}
	fprintf(out, "\t{\n");
	id.output(out, 2);
	fprintf(out, "\t\tname=\"%s\"\n", name.c_str());
	if (leaderID > 0)
	{
		fprintf(out, "\t\tleader=\n");
		fprintf(out, "\t\t{\n");
		fprintf(out, "\t\t\tid=%d\n", leaderID);
		fprintf(out, "\t\t\ttype=37\n");
		fprintf(out, "\t\t}\n");
	}
	fprintf(out, "\t\tlocation=%d\n", location);
	fprintf(out, "\t\tsupplies=1.000\n");
	for (vector<V2Regiment>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		itr->output(out);
	}
	if (isNavy)
	{
		fprintf(out, "\t\tat_sea=%d\n", at_sea);
	}
	fprintf(out, "\t}\n");
}


void V2Army::addRegiment(V2Regiment reg)
{
	regiments.push_back(reg);
}


void V2Army::getRegimentCounts(int counts[num_reg_categories]) const
{
	for (vector<V2Regiment>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		counts[itr->getCategory()]++;
	}
}