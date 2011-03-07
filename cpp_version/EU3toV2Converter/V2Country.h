#include <string>
#include "Parsers\Object.h"
#include "V2Province.h"
#include "V2State.h"
#include "EU3Country.h"
using namespace std;


class V2Country
{
	public:
		void			init(string tag, string countryFile);
		string		getTag();
		void			setSourceCountryIndex(int);
		void			addState(V2State);
		void			setCapital(int);
		void			setcivilized(bool);
		void			output(FILE*);
		int			getSourceCountryIndex();
	private:
		string				tag;
		vector<V2State>	states;
		int					capital;
		bool					civilized;
		int					sourceCountryIndex;
		string				countryFile;
};