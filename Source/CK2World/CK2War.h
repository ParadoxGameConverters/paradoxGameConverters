#ifndef CK2WAR_H_
#define CK2WAR_H_

#include <vector>
#include <string>
using namespace std;

class Object;

class CK2War
{
	public:
		CK2War(Object*);

		string name;

		vector<int> attackers;
		vector<int> defenders;

		bool areEnemies(int lhs, int rhs) const;
};

#endif // CK2WAR_H_