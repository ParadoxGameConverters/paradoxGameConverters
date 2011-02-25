#include <string>
#include "Parsers\Object.h"
#include "V2Province.h"
#include "V2State.h"
#include "EU3Country.h"
using namespace std;


class V2Country
{
	public:
		void			init(string, int);
		string		getTag();
		void			addState(V2State);
		void			setCapital(int);
		void			output(FILE*);
		int			getSourceCountryIndex();
	private:
		string				tag;
		vector<V2State>	states;
		int					capital;
		int					sourceCountryIndex;
};