#include "CK2War.h"
#include "../Parsers/Object.h"
#include <algorithm>


CK2War::CK2War(Object* obj)
{
	vector<Object*> nameObjs = obj->getValue("name");
	if (nameObjs.size() > 0)
		name = nameObjs[0]->getLeaf();

	vector<Object*> attackerObjs = obj->getValue("attacker");
	for (vector<Object*>::iterator itr = attackerObjs.begin(); itr != attackerObjs.end(); ++itr)
	{
		attackers.push_back(atoi((*itr)->getLeaf().c_str()));
	}

	vector<Object*> defenderObjs = obj->getValue("defender");
	for (vector<Object*>::iterator itr = defenderObjs.begin(); itr != defenderObjs.end(); ++itr)
	{
		defenders.push_back(atoi((*itr)->getLeaf().c_str()));
	}
}


bool CK2War::areEnemies(int lhs, int rhs) const
{
	vector<int>::const_iterator lhs_attacking = find(attackers.begin(), attackers.end(), lhs);
	vector<int>::const_iterator rhs_attacking = find(attackers.begin(), attackers.end(), rhs);
	
	// if neither or both are attacking, they're not enemies
	if ((lhs_attacking == attackers.end()) == (rhs_attacking == attackers.end()))
		return false;

	vector<int>::const_iterator lhs_defending = find(defenders.begin(), defenders.end(), lhs);
	vector<int>::const_iterator rhs_defending = find(defenders.begin(), defenders.end(), rhs);

	// if both sides are involved, and they passed the previous check, they're enemies
	if ((lhs_attacking != attackers.end()) || (lhs_defending != defenders.end())
	 && (rhs_attacking != attackers.end()) || (rhs_defending != defenders.end()))
		return true;

	// otherwise not
	return false;
}
