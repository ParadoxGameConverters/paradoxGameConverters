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
		CK2History();
		void init(Object*, map<int, CK2Character*>&);
		date				getWhen();
		CK2Character*	getHolder();
	private:
		date				when;
		CK2Character*	holder;
};


#endif // CK2HISTORY_H_