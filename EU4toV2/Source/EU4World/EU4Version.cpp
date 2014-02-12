#include "EU4Version.h"
#include "..\Log.h"



EU4Version::EU4Version()
{
	first		= 0;
	second	= 0;
	third		= 0;
	fourth	= 0;
	log("Version: %d.%d.%d.%d\n", first, second, third, fourth);
}


EU4Version::EU4Version(Object* obj)
{
	vector<Object*> numObj;
	numObj = obj->getValue("first");
	if (numObj.size() > 0)
	{
		first = atoi(numObj[0]->getLeaf().c_str());
	}
	numObj = obj->getValue("second");
	if (numObj.size() > 0)
	{
		second = atoi(numObj[0]->getLeaf().c_str());
	}
	numObj = obj->getValue("third");
	if (numObj.size() > 0)
	{
		third = atoi(numObj[0]->getLeaf().c_str());
	}
	numObj = obj->getValue("forth");
	if (numObj.size() > 0)
	{
		fourth = atoi(numObj[0]->getLeaf().c_str());
	}
	log("Version: %d.%d.%d.%d\n", first, second, third, fourth);
}


bool EU4Version::operator< (EU4Version& rhs)
{
	if (rhs.first > first)
	{
		return true;
	}
	else if ((rhs.first == first) && (rhs.second > second))
	{
		return true;
	}
	else if ((rhs.first == first) && (rhs.second == second) && (rhs.third > third))
	{
		return true;
	}
	else if ((rhs.first == first) && (rhs.second == second) && (rhs.third == third) && (rhs.fourth > fourth))
	{
		return true;
	}
	else
	{
		return false;
	}
}