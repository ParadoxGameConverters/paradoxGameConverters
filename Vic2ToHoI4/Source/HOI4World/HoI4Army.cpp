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



#include "HoI4Army.h"
#include "Log.h"
#include "../Configuration.h"
#include "ParadoxParserUTF8.h"
#include <sstream>



HoI4RegimentType::HoI4RegimentType(wstring type)
{
	wstring	filename	= Configuration::getHoI4Path() + L"\\tfh\\units\\" + type + L".txt";
	Object*	obj		= parser_UTF8::doParseFile(filename.c_str());
	obj					= obj->getLeaves()[0];
	name					= obj->getKey();

	vector<Object*> usableByObject = obj->getValue(L"usable_by");
	if (usableByObject.size() > 0)
	{
		vector<wstring> tokens = usableByObject[0]->getTokens();
		for (auto tokensItr: tokens)
		{
			usableBy.insert(tokensItr);
		}
	}

	wstring unit_type = obj->getLeaf(L"type");
	if (unit_type == L"air")
	{
		force_type = air;
	}
	else if (unit_type == L"land")
	{
		force_type = land;
	}
	else if (unit_type == L"naval")
	{
		force_type = navy;
	}
	else
	{
		LOG(LogLevel::Error) << "Possible bad unit type in " << filename << "!";
	}

	max_strength			= _wtoi(obj->getLeaf(L"max_strength").c_str());
	practicalBonus			= obj->getLeaf(L"on_completion");
	practicalBonusFactor	= _wtof(obj->getLeaf(L"completion_size").c_str());
}


void HoI4Regiment::output(FILE* out, int indentlevel) const
{
	wstring indents(indentlevel, '\t');
	const wchar_t* indent = indents.c_str();

	switch (type.getForceType())
	{
		case land:
			fwprintf(out, L"%sregiment = ", indent);
			break;
		case navy:
			fwprintf(out, L"%sship = ", indent);
			break;
		case air:
			fwprintf(out, L"%swing = ", indent);
			break;
	}
	fwprintf(out, L"{ ");
	fwprintf(out, L"type = %s ", type.getName().c_str());
	fwprintf(out, L"name = \"%s\" ", name.c_str());
	fwprintf(out, L"historical_model = %d ", historicalModel);
	fwprintf(out, L"}\n");
}


void HoI4Regiment::outputIntoProductionQueue(FILE* out) const
{
	fwprintf(out, L"\t%s = {\n", type.getName().c_str());
	fwprintf(out, L"\t\tname = \"%s\"\n", name.c_str());
	fwprintf(out, L"\t\thistorical_model = %d\n", historicalModel);
	fwprintf(out, L"\t}\n");
}


HoI4RegGroup::HoI4RegGroup()
{
	leaderID				= 0;
	command_level		= division;
	productionQueue	= false;
}


void HoI4RegGroup::output(FILE* out, int indentlevel /* = 0*/) const
{
	if (isEmpty())
	{
		LOG(LogLevel::Warning) << "Regiment Group " << name << " has no regiments after conversion; skipping.";
		return;
	}

	wstring indents(indentlevel, '\t');
	const wchar_t* indent = indents.c_str();

	switch (force_type)
	{
		case land:
			switch (command_level)
			{
				case division:
					fwprintf(out, L"%sdivision = {\n", indent);
					break;
				case corps:
					fwprintf(out, L"%scorps = {\n", indent);
					break;
				case army:
					fwprintf(out, L"%sarmy = {\n", indent);
					break;
				case armygroup:
					fwprintf(out, L"%sarmygroup = {\n", indent);
					break;
				case theatre:
					fwprintf(out, L"%stheatre = {\n", indent);
					break;
				default:
					LOG(LogLevel::Error) << "Internal error: unexpected army command level!";
			}
			break;
		case navy:
			fwprintf(out, L"%snavy = {\n", indent);
			break;
		case air:
			fwprintf(out, L"%sair = {\n", indent);
			break;
		default:
			LOG(LogLevel::Error) << "Internal error: unexpected force type!";
	}
	fwprintf(out, L"%s\tname=\"%s\"\n", indent, name.c_str());
	if (force_type == air || (force_type == navy && !at_sea))
	{
		fwprintf(out, L"%s\tbase = %d\n", indent, location);
	}
	if (leaderID)
	{
		fwprintf(out, L"%s\tleader = %d\n", indent, leaderID);
	}
	fwprintf(out, L"%s\tlocation = %d\n", indent, location);

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
			fwprintf(out, L"%s\tis_reserve = yes\n", indent);
		}
	}

	for (auto itr: regiments)
	{
		itr.output(out, indentlevel + 1);
	}

	for (auto itr: children)
	{
		itr.output(out, indentlevel + 1);
	}

	fwprintf(out, L"%s}\n", indent);
}


void HoI4RegGroup::outputIntoProductionQueue(FILE* out, const wstring& tag) const
{
	if (isEmpty())
	{
		LOG(LogLevel::Warning) << "Regiment Group " << name << " has no regiments after conversion; skipping.";
		return;
	}

	if (force_type == land && command_level == division) // Build as a division
	{
		fwprintf(out, L"military_construction = {\n");
		fwprintf(out, L"\tcountry = %s\n", tag.c_str());
		fwprintf(out, L"\t\tname = \"%s\"\n", name.c_str());
		for (auto itr: regiments)
		{
			itr.outputIntoProductionQueue(out);
		}

		fwprintf(out, L"\tcost = 0\n");
		fwprintf(out, L"\tprogress = 0\n");
		fwprintf(out, L"\tduration = 0\n"); // This makes the unit already or almost complete in the construction queue
		fwprintf(out, L"}\n");
	}
	else
	{
		for (auto itr: regiments)
		{
			fwprintf(out, L"military_construction = {\n");
			fwprintf(out, L"\tcountry = %s\n", tag.c_str());
			itr.outputIntoProductionQueue(out);
			fwprintf(out, L"\tcost = 0\n");
			fwprintf(out, L"\tprogress = 0\n");
			fwprintf(out, L"\tduration = 0\n"); // This makes the unit already or almost complete in the construction queue
			fwprintf(out, L"}\n");
		}
	}

	for (auto itr: children)
	{
		itr.outputIntoProductionQueue(out, tag);
	}
}


HoI4RegGroup HoI4RegGroup::createChild()
{
	HoI4RegGroup newChild;
	newChild.setLocation(location);
	newChild.setForceType(force_type);
	newChild.setAtSea(at_sea);
	newChild.setCommandLevel((CommandLevel)(command_level - 1));
	newChild.setName();
	return newChild;
}


static int hqCount;
void HoI4RegGroup::resetHQCounts()
{
	hqCount = 0;
}


static wstring CardinalToOrdinal(int cardinal)
{
	int hundredRem	= cardinal % 100;
	int tenRem		= cardinal % 10;
	if (hundredRem - tenRem == 10)
	{
		return L"th";
	}

	switch (tenRem)
	{
		case 1:
			return L"st";
			break;
		case 2:
			return L"nd";
			break;
		case 3:
			return L"rd";
			break;
		default:
			return L"th";
	}
}


void HoI4RegGroup::createHQs(HoI4RegimentType hqType)
{
	if (command_level > division)
	{
		HoI4Regiment hq;
		wstringstream regname;
		regname << ++hqCount << CardinalToOrdinal(hqCount) << L" Headquarters Brigade";
		hq.setName(regname.str());
		hq.setType(hqType);
		hq.setHistoricalModel(0);
		regiments.push_back(hq);

		for (auto itr: children)
		{
			itr.createHQs(hqType);
		}
	}
}


static map<CommandLevel, int> regGroupNameCounts;
void HoI4RegGroup::resetRegGroupNameCounts()
{
	for (int i = 0; i < theatre; ++i)
	{
		regGroupNameCounts[static_cast<CommandLevel>(i)] = 0;
	}
}


void HoI4RegGroup::setName()
{
	if (name != L"")
	{
		LOG(LogLevel::Warning) << "resetting name for " << name << "!";
		}

	int index = ++regGroupNameCounts[command_level];
	wstringstream newname;
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


bool HoI4RegGroup::addChild(HoI4RegGroup newChild, bool allowPromote)
{
	if ((allowPromote) && (command_level <= newChild.getCommandLevel()))
	{
		command_level = static_cast<CommandLevel>(newChild.getCommandLevel() + 1);
	}
	else if (command_level <= newChild.getCommandLevel())
	{
		return false;
	}

	// Am I one command level above the child and don't have too many children?
	if ((children.size() < 5) && (command_level == (newChild.getCommandLevel() + 1)))
	{
		children.push_back(newChild);
		return true;
	}

	// can an existing child take this?
	for (auto child: children)
	{
		if (child.addChild(newChild, false))
		{
			return true;
		}
	}

	// can I create a new child (<5 current children, or I am an armygroup)?
	if (children.size() < 5 || command_level == armygroup)
	{
		// add child
		HoI4RegGroup newerChild = createChild();
		if (newerChild.addChild(newChild, false))
		{
			children.push_back(newerChild);
			return true;
		}
	}

	// can't add this group; can I promote myself?
	if (allowPromote)
	{
		command_level = static_cast<CommandLevel>(command_level + 1);

		// split current children between two new intermediate children if appropriate
		HoI4RegGroup child1 = createChild();
		HoI4RegGroup child2 = createChild();
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
				child1.regiments.push_back(regiments[i]);
			}
			regiments.clear();
		}
		children.push_back(child1);
		if (!child2.isEmpty())
		{
			children.push_back(child2);
		}

		// try again: can an existing child take this?
		for (auto itr: children)
		{
			if (itr.addChild(newChild, false))
			{
				return true;
			}
		}

		// try again: can I create a new child (<5 current children, or I am an armygroup)?
		if (children.size() < 5 || command_level == armygroup)
		{
			// add child
			HoI4RegGroup newerChild = createChild();
			if (newerChild.addChild(newChild, false))
			{
				children.push_back(newerChild);
				return true;
			}
		}
	}

	children.push_back(newChild);
	return false;
}


bool HoI4RegGroup::addRegiment(HoI4Regiment reg, bool allowPromote)
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
		for (auto itr: children)
		{
			if (itr.addRegiment(reg, false))
			{
				return true;
			}
		}
		// no: can I create a new child (<5 current children, or I am an armygroup)?
		if (children.size() < 5 || command_level == armygroup)
		{
			// add child
			HoI4RegGroup newChild = createChild();

			// rebalance grandchildren: give the new child one grandchild from every existing child
			if (command_level > corps)
			{
				for (auto itr = children.begin(); itr != children.end(); ++itr)
				{
					vector<HoI4RegGroup>::iterator grandchild = --itr->children.end();
					HoI4RegGroup temp = *(grandchild);
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
			for (auto itr: children)
			{
				if (itr.addRegiment(reg, false))
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
		HoI4RegGroup child1 = createChild();
		HoI4RegGroup child2 = createChild();
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
		for (auto itr: children)
		{
			if (itr.addRegiment(reg, false))
			{
				return true;
			}
		}
	}

	// nope, really can't add it
	return false;
}


int HoI4RegGroup::size() const 
{
	unsigned size = regiments.size();

	for (auto itr: children)
	{
		size += itr.size();
	}

	return size;
}


void HoI4RegGroup::undoPracticalAddition(map<wstring, double>& practicals) const
{
	for (auto itr: regiments)
	{
		practicals[itr.getType().getPracticalBonus()] -= itr.getType().getPracticalBonusFactor();
	}

	for (auto itr: children)
	{
		itr.undoPracticalAddition(practicals);
	}
}
