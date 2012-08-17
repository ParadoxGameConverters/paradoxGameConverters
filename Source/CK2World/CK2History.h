#ifndef CK2HISTORY_H_
#define CK2HISTORY_H_


#include <map>
#include "..\Date.h"
using namespace std;



class Object;
class CK2Character;

class CK2History
{
	public:
		CK2History(Object*, map<int, CK2Character*>&);

		date				getWhen()	const { return when; };
		CK2Character*	getHolder()	const { return holder; };
	private:
		date				when;
		CK2Character*	holder;
};


#endif // CK2HISTORY_H_