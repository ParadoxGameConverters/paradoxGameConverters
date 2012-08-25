#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
#include "..\Date.h"
#include "..\Mappers.h"
using namespace std;


class CK2Title;
class EU3Ruler;
class EU3History;
class EU3Province;
class CK2Province;

class EU3Country
{
	public:
		EU3Country(string tag, string countryFile, date startDate);

		void		output(FILE*);
		
		void		convert(const CK2Title*, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping inverseProvinceMap);
		void		addAcceptedCultures();
		void		determineGovernment();

		void		addProvince(EU3Province* province)	{ provinces.push_back(province); };

		string	getTag()			const { return tag; };
		string	getTechGroup()	const { return techGroup; };

	private:
		const CK2Title*		src;
		vector<EU3Province*>	provinces;

		string					tag;
		string					historyFile;
		string					government;
		string					primaryCulture;
		vector<string>			acceptedCultures;
		string					religion;
		EU3Ruler*				monarch;
		EU3Ruler*				heir;
		EU3Ruler*				regent;
		vector<EU3History*>	history;
		vector<EU3Ruler*>		previousMonarchs;
		string					techGroup;
		int						capital;
};



#endif	// EU3COUNTRY_H_