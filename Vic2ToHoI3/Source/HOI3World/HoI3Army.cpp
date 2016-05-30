/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "HoI3Army.h"
#include "Log.h"
#include "../Configuration.h"
#include "ParadoxParser.h"
#include <sstream>



HoI3RegimentType::HoI3RegimentType(string type)
{
	string	filename	= Configuration::getHoI3Path() + "\\tfh\\units\\" + type + ".txt";
	Object*	obj		= doParseFile(filename.c_str());
	obj					= obj->getLeaves()[0];
	name					= obj->getKey();

	vector<Object*> usableByObject = obj->getValue("usable_by");
	if (usableByObject.size() > 0)
	{
		vector<string> tokens = usableByObject[0]->getTokens();
		for (auto tokensItr: tokens)
		{
			usableBy.insert(tokensItr);
		}
	}

	string unit_type = obj->getLeaf("type");
	if (unit_type == "air")
	{
		force_type = air;
	}
	else if (unit_type == "land")
	{
		force_type = land;
	}
	else if (unit_type == "naval")
	{
		force_type = navy;
	}
	else
	{
		LOG(LogLevel::Error) << "Possible bad unit type in " << filename << "!";
	}

	max_strength			= atoi(obj->getLeaf("max_strength").c_str());
	practicalBonus			= obj->getLeaf("on_completion");
	practicalBonusFactor	= atof(obj->getLeaf("completion_size").c_str());
}


void HoI3Regiment::output(FILE* out, int indentlevel) const
{
	string indents(indentlevel, '\t');
	const char* indent = indents.c_str();

	switch (type.getForceType())
	{
		case land:
			fprintf(out, "%sregiment = ", indent);
			break;
		case navy:
			fprintf(out, "%sship = ", indent);
			break;
		case air:
			fprintf(out, "%swing = ", indent);
			break;
	}
	fprintf(out, "{ ");
	fprintf(out, "type = %s ", type.getName().c_str());
	fprintf(out, "name = \"%s\" ", name.c_str());
	fprintf(out, "historical_model = %d ", historicalModel);
	fprintf(out, "}\n");
}


void HoI3Regiment::outputIntoProductionQueue(FILE* out) const
{
	fprintf(out, "\t%s = {\n", type.getName().c_str());
	fprintf(out, "\t\tname = \"%s\"\n", name.c_str());
	fprintf(out, "\t\thistorical_model = %d\n", historicalModel);
	fprintf(out, "\t}\n");
}


HoI3RegGroup::HoI3RegGroup()
{
	leaderID				= 0;
	command_level		= division;
	productionQueue	= false;
}


void HoI3RegGroup::output(FILE* out, int indentlevel /* = 0*/) const
{
	if (isEmpty())
	{
		LOG(LogLevel::Warning) << "Regiment Group " << name << " has no regiments after conversion; skipping.";
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
					fprintf(out, "%sdivision = {\n", indent);
					break;
				case corps:
					fprintf(out, "%scorps = {\n", indent);
					break;
				case army:
					fprintf(out, "%sarmy = {\n", indent);
					break;
				case armygroup:
					fprintf(out, "%sarmygroup = {\n", indent);
					break;
				case theatre:
					fprintf(out, "%stheatre = {\n", indent);
					break;
				default:
					LOG(LogLevel::Error) << "Internal error: unexpected army command level!";
			}
			break;
		case navy:
			fprintf(out, "%snavy = {\n", indent);
			break;
		case air:
			fprintf(out, "%sair = {\n", indent);
			break;
		default:
			LOG(LogLevel::Error) << "Internal error: unexpected force type!";
	}
	fprintf(out, "%s\tname=\"%s\"\n", indent, name.c_str());
	if (force_type == air || (force_type == navy && !at_sea))
	{
		fprintf(out, "%s\tbase = %d\n", indent, location);
	}
	if (leaderID)
	{
		fprintf(out, "%s\tleader = %d\n", indent, leaderID);
	}
	fprintf(out, "%s\tlocation = %d\n", indent, location);

	if ((force_type == land) && (command_level == division))
	{
		bool reserve = false;
		for (auto regiment : regiments)
		{
			if (regiment.isReserve())
			{
				reserve = true;
			}
		}

		if (reserve)
		{
			fprintf(out, "%s\tis_reserve = yes\n", indent);
		}
	}

	for (vector<HoI3Regiment>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		itr->output(out, indentlevel + 1);
	}

	for (vector<HoI3RegGroup>::const_iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		itr->output(out, indentlevel + 1);
	}

	fprintf(out, "%s}\n", indent);
}


void HoI3RegGroup::outputIntoProductionQueue(FILE* out, const string& tag) const
{
	if (isEmpty())
	{
		LOG(LogLevel::Warning) << "Regiment Group " << name << " has no regiments after conversion; skipping.";
		return;
	}

	if (force_type == land && command_level == division) // Build as a division
	{
		fprintf(out, "military_construction = {\n");
		fprintf(out, "\tcountry = %s\n", tag.c_str());
		fprintf(out, "\t\tname = \"%s\"\n", name.c_str());
		for (vector<HoI3Regiment>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
		{
			itr->outputIntoProductionQueue(out);
		}

		fprintf(out, "\tcost = 0\n");
		fprintf(out, "\tprogress = 0\n");
		fprintf(out, "\tduration = 0\n"); // This makes the unit already or almost complete in the construction queue
		fprintf(out, "}\n");
	}
	else
	{
		for (vector<HoI3Regiment>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
		{
			fprintf(out, "military_construction = {\n");
			fprintf(out, "\tcountry = %s\n", tag.c_str());
			itr->outputIntoProductionQueue(out);
			fprintf(out, "\tcost = 0\n");
			fprintf(out, "\tprogress = 0\n");
			fprintf(out, "\tduration = 0\n"); // This makes the unit already or almost complete in the construction queue
			fprintf(out, "}\n");
		}
	}

	for (vector<HoI3RegGroup>::const_iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		itr->outputIntoProductionQueue(out, tag);
	}
}


HoI3RegGroup HoI3RegGroup::createChild()
{
	HoI3RegGroup newChild;
	newChild.setLocation(location);
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


static string CardinalToOrdinal(int cardinal)
{
	int hundredRem	= cardinal % 100;
	int tenRem		= cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return "th";
	}

	switch (tenRem)
	{
		case 1:
			return "st";
			break;
		case 2:
			return "nd";
			break;
		case 3:
			return "rd";
			break;
		default:
			return "th";
	}
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
		regGroupNameCounts[static_cast<CommandLevel>(i)] = 0;
	}
}


void HoI3RegGroup::setName()
{
	if (name != "")
	{
		LOG(LogLevel::Warning) << "resetting name for " << name << "!";
		}

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
	{
		LOG(LogLevel::Error) << "tried to externally add a child to a non-theatre army group!";
	}

	children.push_back(newChild);
}


bool HoI3RegGroup::addRegiment(HoI3Regiment reg, bool allowPromote)
{
	// only add ships to navies and regiments to armies
	if (force_type != reg.getForceType())
	{
		LOG(LogLevel::Error) << "attempted to add unit to parent of incorrect type!";
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
			{
				return true;
			}
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
					{
						break;
					}
				}
			}
			children.push_back(newChild);

			// try again: can an existing child take this?
			for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
			{
				if (itr->addRegiment(reg, false))
				{
					return true;
				}
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
		{
			command_level = CommandLevel((int)command_level + 1);
		}
		else
		{
			LOG(LogLevel::Error) << "Internal error : tried to promote an army group!";
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
				{
					child1.children.push_back(children[i]);
				}
				else
				{
					child2.children.push_back(children[i]);
				}
			}
			children.clear();
		}
		else // NEW command_level == corps (originally a division)
		{
			for (size_t i = 0; i < regiments.size(); ++i)
			{
				if (i % 2 == 0)
				{
					child1.regiments.push_back(regiments[i]);
				}
				else
				{
					child2.regiments.push_back(regiments[i]);
				}
			}
			regiments.clear();
		}
		children.push_back(child1);
		children.push_back(child2);

		// try again: can an existing child take this?
		for (vector<HoI3RegGroup>::iterator itr = children.begin(); itr != children.end(); ++itr)
		{
			if (itr->addRegiment(reg, false))
			{
				return true;
			}
		}
	}

	// nope, really can't add it
	return false;
}


int HoI3RegGroup::size() const 
{
	unsigned size = regiments.size();

	for (vector<HoI3RegGroup>::const_iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		size += itr->size();
	}

	return size;
}


void HoI3RegGroup::undoPracticalAddition(map<string, double>& practicals) const
{
	for (vector<HoI3Regiment>::const_iterator itr = regiments.begin(); itr != regiments.end(); ++itr)
	{
		practicals[itr->getType().getPracticalBonus()] -= itr->getType().getPracticalBonusFactor();
	}

	for (vector<HoI3RegGroup>::const_iterator itr = children.begin(); itr != children.end(); ++itr)
	{
		itr->undoPracticalAddition(practicals);
	}
}
