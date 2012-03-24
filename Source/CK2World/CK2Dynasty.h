#ifndef CK2DYNASTY_H_
#define CK2DYNASTY_H_


#include <string>
#include "..\Parsers\Object.h"
using namespace std;



class CK2Dynasty
{
	public:
		void		init(Object*);
		int		getNum();
		string	getName();
	private:
		int	 num;
		string name;
};



#endif // CK2DYNASTY_H_