#include <string>
#include "Parsers\Object.h"
#include "V2Province.h"
#include "V2State.h"
#include "V2Inventions.h"
#include "EU3Country.h"
using namespace std;




class V2Country
{
	public:
		void			init(string tag, string countryFile, vector<int> parties);
		string		getTag();
		void			setSourceCountryIndex(int);
		void			addState(V2State);
		void			setCapital(int);
		void			setcivilized(bool);
		void			setPrimaryCulture(string);
		void			addAcceptedCulture(string);
		void			setReligion(string);
		void			output(FILE*);
		int			getSourceCountryIndex();
		void			setArmyTech(int);
		void			setNavyTech(int);
		void			setCommerceTech(int);
		void			setIndustryTech(int);
		void			setCultureTech(int);
	private:
		void outputTech(FILE*);
		void outputInventions(FILE*);

		string				tag;
		vector<V2State>	states;
		int					capital;
		bool					civilized;
		string				primaryCulture;
		vector<string>		acceptedCultures;
		string				religion;
		vector<int>			parties;
		int					sourceCountryIndex;
		string				countryFile;
		int					armyTech;
		int					navyTech;
		int					commerceTech;
		int					industryTech;
		int					cultureTech;
		int					inventions[naval_exercises];
};