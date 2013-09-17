#include "EU4Loan.h"
#include "Parsers\Object.h"



EU4Loan::EU4Loan(Object* obj)
{
	vector<Object*> moneyObj = obj->getValue("lender");
	if (moneyObj.size() > 0)
	{
		lender = moneyObj[0]->getLeaf();
	}
	else
	{
		lender = "";
	}
	moneyObj = obj->getValue("interest");
	if (moneyObj.size() > 0)
	{
		interest = atof(moneyObj[0]->getLeaf().c_str());
	}
	else
	{
		interest = 0.0;
	}
	moneyObj = obj->getValue("amount");
	if (moneyObj.size() > 0)
	{
		amount = atof(moneyObj[0]->getLeaf().c_str());
	}
	else
	{
		amount = 0.0;
	}
}