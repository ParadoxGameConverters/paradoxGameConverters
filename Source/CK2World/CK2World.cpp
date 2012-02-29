#include "CK2World.h"



void CK2World::init(Object* obj)
{
	// get conversion date
	vector<Object*> dateObj = obj->getValue("date");
	if (dateObj.size() > 0)
	{
		date newDate( dateObj[0]->getLeaf() );
		endDate = newDate;
	}
	else
	{
		date newDate("1399.10.14");
	}

	// get titles
	vector<CK2Title> titles;
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( (key.substr(0, 2) == "e_") || (key.substr(0, 2) == "k_") || (key.substr(0, 2) == "d_") || (key.substr(0, 2) == "c_") || (key.substr(0, 2) == "b_") )
		{
			CK2Title newTitle;
			newTitle.init(leaves[i]);
			titles.push_back(newTitle);
		}
	}
}


date CK2World::getEndDate()
{
	return endDate;
}