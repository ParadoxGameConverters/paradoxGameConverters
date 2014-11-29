/*Copyright (c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "V2Army.h"
#include "../Log.h"
#include "../Parsers/Parser.h"



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
			LOG(LogLevel::Warning) << "Unexpected regiment category " << rc;
			break;
	}
}


V2Regiment::V2Regiment(Object* obj)
{
	string key = obj->getKey();
	isShip = (key == "ship");

	std::vector<Object*> objName = obj->getValue("name");
	(objName.size() > 0) ? name = objName[0]->getLeaf() : name = "";

	std::vector<Object*> objType = obj->getValue("type");
	if (objType.size() > 0)
	{
		type = objType[0]->getLeaf();
	}
	else
	{
		LOG(LogLevel::Warning) << "Regiment or Ship " << name << " has no type";
		type = "";
	}

	std::vector<Object*> objStr = obj->getValue("strength");
	if (objStr.size() > 0)
	{
		strength = atof(objStr[0]->getLeaf().c_str());
	}
	else
	{
		strength = 0.0;
	}

	objStr = obj->getValue("organisation");
	if (objStr.size() > 0)
	{
		organization = atof(objStr[0]->getLeaf().c_str());
	}
	else
	{
		organization = 0.0;
	}

	objStr = obj->getValue("experience");
	if (objStr.size() > 0)
	{
		experience = atof(objStr[0]->getLeaf().c_str());
	}
	else
	{
		experience = 0.0;
	}

	/* -- not needed?
	std::vector<Object*> objPop = obj->getValue("pop");
	if (objPop.size() > 0)
	{
	objPop = objPop[0]->getValue("id");
	popID = atoi(objPop[0]->getLeaf().c_str());
	}
	else
	{
	popID = 0;
	}
	*/
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


V2Army::V2Army(Object* obj)
{
	string key = obj->getKey();
	isNavy = (key == "navy");

	std::vector<Object*> objName = obj->getValue("name");
	(objName.size() > 0) ? name = objName[0]->getLeaf() : name = "";

	std::vector<Object*> objLoc = obj->getValue("location");
	if (objLoc.size() > 0)
	{
		location = atoi(objLoc[0]->getLeaf().c_str());
	}
	else
	{
		LOG(LogLevel::Warning) << "Army or Navy " << name << " has no location";
		location = -1;
	}

	std::vector<Object*> objAtSea = obj->getValue("at_sea");
	(objAtSea.size() > 0) ? at_sea = atoi(objAtSea[0]->getLeaf().c_str()) : at_sea = 0;

	regiments.clear();
	std::vector<Object*> objRegs = obj->getValue("regiment");
	for (vector<Object*>::iterator itr = objRegs.begin(); itr != objRegs.end(); ++itr)
	{
		//V2Regiment* reg = new V2Regiment(*itr);
		regiments.push_back(V2Regiment(*itr));
	}
	std::vector<Object*> objShips = obj->getValue("ship");
	for (vector<Object*>::iterator itr = objShips.begin(); itr != objShips.end(); ++itr)
	{
		//V2Regiment* reg = new V2Regiment(*itr);
		regiments.push_back(V2Regiment(*itr));
	}

	std::vector<Object*> objLeader = obj->getValue("leader");
	if (objLeader.size() > 0)
	{
		objLeader = objLeader[0]->getValue("id");
		leaderID = atoi(objLeader[0]->getLeaf().c_str());
	}
	else
	{
		leaderID = 0;
	}

	std::vector<Object*> objSupp = obj->getValue("supplies");
	if (objSupp.size() > 0)
	{
		supplies = atof(objSupp[0]->getLeaf().c_str());
	}
	else
	{
		supplies = 0.0;
	}
}


void V2Army::output(FILE* out) const
{
	if (regiments.size() == 0)
	{
		LOG(LogLevel::Debug) << "Army " << name << " has no regiments after conversion; skipping";
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