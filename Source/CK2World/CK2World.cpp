#include "CK2World.h"



void CK2World::init(Object* obj)
{
	// get conversion date
	vector<Object*> dateObj = obj[0].getValue("date");
	if (dateObj.size() > 0)
	{
		date newDate( dateObj[0]->getLeaf() );
		endDate = newDate;
	}
	else
	{
		date newDate("1399.10.14");
	}
}


date CK2World::getEndDate()
{
	return endDate;
}