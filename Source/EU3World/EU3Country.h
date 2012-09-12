#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
#include "..\Date.h"
#include "..\Mappers.h"
#include "..\CK2World\CK2Title.h"
using namespace std;


class CK2Title;
class EU3Ruler;
class EU3History;
class EU3Province;
class EU3World;
class CK2Province;

class EU3Country
{
	public:
		EU3Country(EU3World* world, string tag, string countryFile, date startDate);

		void		output(FILE*);
		
		void		convert(const CK2Title*, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping inverseProvinceMap);
		void		determineLearningScore();
		void		addAcceptedCultures();
		void		determineGovernment(const religionGroupMapping& religionGroupMap);
		void		determineEconomy(const cultureGroupMapping& cultureGroups);
		double	getTradeEffeciency();

		void		addProvince(EU3Province* province)	{ provinces.push_back(province); };
		void		setTechGroup(string _techGroup)		{ techGroup = _techGroup; };

		double			getLearningScore()		const { return learningScore; };
		string			getTag()						const { return tag; };
		string			getGovernment()			const { return government; };
		string			getPrimaryCulture()		const { return primaryCulture; };
		vector<string>	getAcceptedCultures()	const { return acceptedCultures; };
		string			getReligion()				const { return religion; };
		string			getTechGroup()				const { return techGroup; };
		int				getCapital()				const { return capital; };
		int				getStability()				const { return stability; };

	private:
		const CK2Title*		src;
		vector<EU3Province*>	provinces;
		double					learningScore;

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
		int						stability;
		double					stabilityInvestment;

		double					estimatedIncome;
		double					estimatedTax;
		double					estimatedTolls;

		bool						daimyo;
		bool						japaneseEmperor;
};



#endif	// EU3COUNTRY_H_