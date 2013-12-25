/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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
