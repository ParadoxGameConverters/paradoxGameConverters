#ifndef CK2DYNASTY_H_
#define CK2DYNASTY_H_


#include <string>
#include <vector>
using namespace std;



class Object;
class CK2Character;

class CK2Dynasty
{
	public:
		CK2Dynasty(Object*);
		CK2Dynasty(int, string);

		void				addMember(CK2Character*);
		CK2Character*	getSenoirityHeir(string genderLaw);

		string			getName()	const { return name; };
	private:
		int							num;
		string						name;
		vector<CK2Character*>	members;
};



#endif // CK2DYNASTY_H_