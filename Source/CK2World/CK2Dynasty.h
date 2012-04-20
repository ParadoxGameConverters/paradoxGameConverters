#ifndef CK2DYNASTY_H_
#define CK2DYNASTY_H_


#include <string>
#include "..\Parsers\Object.h"
using namespace std;



//forward declaration
class CK2Character;

class CK2Dynasty
{
	public:
		void				init(Object*);
		void				init(int, string);
		int				getNum();
		string			getName();
		void				addMember(CK2Character*);
		CK2Character*	getSenoirityHeir(string genderLaw);
	private:
		int							num;
		string						name;
		vector<CK2Character*>	members;
};



#endif // CK2DYNASTY_H_