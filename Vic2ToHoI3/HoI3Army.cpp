#include "HoI3Army.h"
#include "Log.h"
#include "Configuration.h"
#include "Parsers/Parser.h"

#include <sstream>


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

void HoI3Regiment::output(FILE* out, int indentlevel)
{
	string indents(indentlevel, '\t');
	const char* indent = indents.c_str();

	switch (type.getForceType())
	{
	case land:
		fprintf(out, "%sregiment=\n", indent);
		break;
	case navy:
		fprintf(out, "%sship=\n", indent);
		break;
	case air:
		fprintf(out, "%swing=\n", indent);
		break;
	}
	fprintf(out, "%s{\n", indent);
	id.output(out, indentlevel + 1);
	fprintf(out, "%s\tname=\"%s\"\n", indent, name.c_str());
	fprintf(out, "%s\ttype=\"%s\"\n", indent, type.getName().c_str());
	fprintf(out, "%s\torganisation=%f\n", indent, organization);
	fprintf(out, "%s\tstrength=%f\n", indent, strength);
	//XXX: what does "highest=%f" do??
	if (type.getForceType() == land)
		fprintf(out, "%s\tis_reserve=no\n", indent);
	//XXX: upgrade levels go here
	fprintf(out, "%s}\n", indent);
}

static string CardinalToOrdinal(int cardinal)
{
	int hundredRem = cardinal % 100;
	int tenRem = cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
	case 1:
		return "st";
	case 2:
		return "nd";
	case 3:
		return "rd";
	default:
		return "th";
	}
}

HoI3RegGroup::HoI3RegGroup()
{
	leaderID = 0;
	command_level = division;
}

void HoI3RegGroup::output(FILE* out, int indentlevel)
{
	if (isEmpty())
	{
		log("Regiment Group %s has no regiments after conversion; skipping.\n", name.c_str());
		return;
	}

	string indents(indentlevel, '\t');
	const char* indent = indents.c_str();

	switch (force_type)
	{
	case land:
		switch (command_level)
		{
		case division:
			fprintf(out, "%sdivision=\n", indent);
			break;
		case corps:
			fprintf(out, "%scorps=\n", indent);
			break;
		case army:
			fprintf(out, "%sarmy=\n", indent);
			break;
		case armygroup:
			fprintf(out, "%sarmygroup=\n", indent);
			break;
		case theatre:
			fprintf(out, "%stheatre=\n", indent);
			break;
		default:
			log("Internal error: unexpected army command level!\n");
		}
		break;
	case navy:
		fprintf(out, "%snavy=\n", indent);
		break;
	case air:
		fprintf(out, "%sair=\n", indent);
		break;
	default:
		log("Internal error: unexpected force type!\n");
	}
	fprintf(out, "%s{\n", indent);
	id.output(out, indentlevel + 1);
	fprintf(out, "%s\tname=\"%s\"\n", indent, name.c_str());
	if (force_type == air) // probably should assign bases for navies too, but they might be at sea
		fprintf(out, "%s\tbase=%d\n", indent, location);
	if (leaderID)
	{
		fprintf(out, "%s\tleader=\n", indent);
		fprintf(out, "%s\t{\n", indent);
		fprintf(out, "%s\t\tid=%d\n", indent, leaderID);
		fprintf(out, "%s\t\ttype=37\n", indent);
		fprintf(out, "%s\t}\n", indent);
	}
	fprintf(out, "%s\tlocation=%d\n", indent, location);
	fprintf(out, "%s\tis_prioritized=no\n", indent);
	fprintf(out, "%s\tcan_reinforce=yes\n", indent);
	fprintf(out, "%s\tcan_upgrade=yes\n", indent);
	fprintf(out, "%s\tsupplies=%f\n", indent, supplies);
	fprintf(out, "%s\tfuel=%f\n", indent, fuel);
	for (vector<HoI3Regiment>::iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		itr->output(out, indentlevel + 1);
	}
	if (force_type == navy)
		fprintf(out, "%s\tat_sea=%d\n", indent, at_sea);

	for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		itr->output(out, indentlevel + 1);
	}

	fprintf(out, "%s}\n", indent);
}


HoI3RegGroup HoI3RegGroup::createChild()
{
	HoI3RegGroup newChild;
	newChild.setLocation(location);
	newChild.setLocationContinent(location_continent);
	newChild.setFuelSupplies(fuel);
	newChild.setForceType(force_type);
	newChild.setAtSea(at_sea);
	newChild.setCommandLevel((CommandLevel)(command_level - 1));
	newChild.setName();
	return newChild;
}


static int hqCount;

void HoI3RegGroup::resetHQCounts()
{
	hqCount = 0;
}

void HoI3RegGroup::createHQs(HoI3RegimentType hqType)
{
	if (command_level > division)
	{
		HoI3Regiment hq;
		stringstream regname;
		regname << ++hqCount << CardinalToOrdinal(hqCount) << " Headquarters Brigade";
		hq.setName(regname.str());
		hq.setType(hqType);
		hq.setStrength(hqType.getMaxStrength());
		hq.setOrganization(100.0);
		hq.setExperience(0.0);
		regiments.push_back(hq);

		for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
		{
			itr->createHQs(hqType);
		}
	}
}


static map<CommandLevel, int> regGroupNameCounts;

void HoI3RegGroup::resetRegGroupNameCounts()
{
	for (int i = 0; i < theatre; ++i)
	{
		regGroupNameCounts[(CommandLevel)i] = 0;
	}
}


void HoI3RegGroup::setName()
{
	if (name != "")
		log("Warning: resetting name for %s!\n", name.c_str());

	int index = ++regGroupNameCounts[command_level];
	stringstream newname;
	newname << index << CardinalToOrdinal(index) << " ";
	switch (command_level)
	{
	case division:
		newname << "Division";
		break;
	case corps:
		newname << "Corps";
		break;
	case army:
		newname << "Army";
		break;
	case armygroup:
		newname << "Army Group";
		break;
	}
	name = newname.str();
}


void HoI3RegGroup::addChild(HoI3RegGroup newChild)
{
	if (command_level != theatre)
		log("Error: tried to externally add a child to a non-theatre army group!\n");

	children.push_back(newChild);
}


bool HoI3RegGroup::addRegiment(HoI3Regiment reg, bool allowPromote)
{
	// only add ships to navies and regiments to armies
	if (force_type != reg.getForceType())
	{
		log("Error: attempted to add unit to parent of incorrect type!\n");
		return false;
	}

	// build hierarchies only for land
	if (force_type == air || force_type == navy)
	{
		regiments.push_back(reg);
		return true;
	}

	// only divisions can have non-HQ regiments
	// only non-divisions can have RegGroup children
	if (command_level > division)
	{
		// can an existing child take this?
		for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
		{
			if (itr->addRegiment(reg, false))
				return true;
		}
		// no: can I create a new child (<5 current children, or I am an armygroup)?
		if (children.size() < 5 || command_level == armygroup)
		{
			// add child
			HoI3RegGroup newChild = createChild();

			// rebalance grandchildren: give the new child one grandchild from every existing child
			if (command_level > corps)
			{
				for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
				{
					vector<HoI3RegGroup>::iterator grandchild = --itr->children.end();
					HoI3RegGroup temp = *(grandchild);
					itr->children.erase(grandchild);
					newChild.children.push_back(temp);
					// don't allow more than 4 grandchildren to be transferred (only a problem if I am an armygroup)
					if (newChild.children.size() >= 4)
						break;
				}
			}
			children.push_back(newChild);

			// try again: can an existing child take this?
			for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
			{
				if (itr->addRegiment(reg, false))
					return true;
			}
		}
	}
	else
	{
		if (regiments.size() < 4)
		{
			regiments.push_back(reg);
			return true;
		}
	}

	// can't add this regiment; can I promote myself?
	if (allowPromote)
	{
		// only promote up to army group
		if (command_level < armygroup)
			command_level = CommandLevel((int)command_level + 1);
		else
		{
			log("Internal error: tried to promote an army group!\n");
			return false; // should never get here
		}

		// split current children between two new intermediate children
		HoI3RegGroup child1 = createChild();
		HoI3RegGroup child2 = createChild();
		if (command_level > corps) // original command level was higher than division
		{
			for (size_t i = 0; i < children.size(); ++i)
			{
				if (i % 2 == 0)
					child1.children.push_back(children[i]);
				else
					child2.children.push_back(children[i]);
			}
			children.clear();
		}
		else // NEW command_level == corps (originally a division)
		{
			for (size_t i = 0; i < regiments.size(); ++i)
			{
				if (i % 2 == 0)
					child1.regiments.push_back(regiments[i]);
				else
					child2.regiments.push_back(regiments[i]);
			}
			regiments.clear();
		}
		children.push_back(child1);
		children.push_back(child2);

		// try again: can an existing child take this?
		for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
		{
			if (itr->addRegiment(reg, false))
				return true;
		}
	}

	// nope, really can't add it
	return false;
}
