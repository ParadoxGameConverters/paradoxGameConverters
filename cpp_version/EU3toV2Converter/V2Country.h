#include <string>
#include "Parsers\Object.h"
#include "V2Province.h"
#include "V2State.h"
using namespace std;


class V2Country
{
	public:
		void		init(string);
		string	getTag();
		void		addState(V2State);
		void		output(FILE*);
	private:
		string				tag;
		vector<V2State>	states;
};