#ifndef CK2CHARACTER_H_
#define CK2CHARACTER_H_


#include <string>
#include "..\Parsers\Object.h"
using namespace std;


class CK2Character
{
	public:
		void init(Object*);
		string getName();
	private:
		string name;
		string religion;
		string culture;
};







#endif // CK2CHARACTER_H_