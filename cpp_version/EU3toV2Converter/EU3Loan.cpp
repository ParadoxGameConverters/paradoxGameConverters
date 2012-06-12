#include "EU3Loan.h"
#include "Parsers\Object.h"



EU3Loan::EU3Loan()
{
	lender	= "";
	interest	= -1;
	amount	= -1;
}


void EU3Loan::init(Object* obj)
{
	vector<Object*> moneyObj = obj->getValue("interest");
	if (moneyObj.size() > 0)
	{
		interest = atof(moneyObj[0]->getLeaf().c_str());
	}
	moneyObj = obj->getValue("amount");
	if (moneyObj.size() > 0)
	{
		amount = atof(moneyObj[0]->getLeaf().c_str());
	}
	moneyObj = obj->getValue("lender");
	if (moneyObj.size() > 0)
	{
		lender = moneyObj[0]->getLeaf();
	}
}