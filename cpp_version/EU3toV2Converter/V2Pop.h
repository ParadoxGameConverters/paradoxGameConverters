#ifndef V2POP_H_
#define V2POP_H_

#include <string>
using namespace std;


class V2Pop
{
	public:
		V2Pop();
		void		setType(string);
		void		setSize(int);
		void		setCulture(string);
		void		setReligion(string);
		int		getSize();
		string	getType();
		void		output(FILE*);
	private:

		int		id;
		string	type;
		int		size;
		string	culture;
		string	religion;
};




#endif