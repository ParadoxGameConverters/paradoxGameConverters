#include "V2Leader.h"



V2Leader::V2Leader(Object* obj)
{
	name = obj->getLeaf("name");
	if (name[0] == '\"')
	{
		name	= name.substr(1, name.length() - 2);
	}

	type = obj->getLeaf("type");

	personality	= obj->getLeaf("personality");
	if (personality[0] == '\"')
	{
		personality	= personality.substr(1, personality.length() - 2);
	}

	background	= obj->getLeaf("background");
	if (background[0] == '\"')
	{
		background	= background.substr(1, background.length() - 2);
	}

	prestige = atof(obj->getLeaf("prestige").c_str());
}