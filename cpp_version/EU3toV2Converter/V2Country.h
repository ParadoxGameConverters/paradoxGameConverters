#include <string>
#include "Parsers\Object.h"
#include "V2Province.h"
#include "V2State.h"
#include "EU3Country.h"
using namespace std;


class V2Country
{
	public:
		void			init(string, EU3Country*);
		string		getTag();
		void			addState(V2State);
		void			setCapital(int);
		void			output(FILE*);
		EU3Country*	getSourceCountry();
	private:
		string				tag;
		vector<V2State>	states;
		int					capital;
		EU3Country*			sourceCountry;
};