#ifndef CK2TITLE_H_
#define CK2TITLE_H_


#include "..\mappers.h"
#include <vector>
#include <map>
using namespace std;


class Object;
class CK2World;
class CK2Character;
class CK2History;
class CK2Barony;
class CK2BuildingFactory;
class EU3Country;

class CK2Title
{
	public:
		CK2Title(string _titleString);
		void	init(Object*, map<int, CK2Character*>&, const CK2BuildingFactory* buildingFactory);

		void						setLiege(CK2Title*);
		void						addToHRE();
		void						determineHeir(map<int, CK2Character*>&);
		void						setHeir(CK2Character*);
		void						setDeJureLiege(const map<string, CK2Title*>& titles);
		void						addDeJureVassals(vector<Object*>, map<string, CK2Title*>& titles, CK2World* world);
		void						removeDeJureVassal(CK2Title* vassal);

		void						getCultureWeights(map<string, int>& cultureWeights, const cultureMapping& cultureMap) const;

		void						addDeJureVassal(CK2Title* vassal) { deJureVassals.push_back(vassal); };
		void						setDstCountry(EU3Country* _dstCountry) { dstCountry = _dstCountry; };

		string					getTitleString()		const { return titleString; };
		CK2Character*			getHolder()				const { return holder; };
		CK2Character*			getLastHolder()		const;
		CK2Character*			getHeir()				const { return heir; };
		string					getSuccessionLaw()	const { return successionLaw; };
		string					getCA()					const { return CA; };
		vector<CK2History*>	getHistory()			const { return history; };
		string					getLiegeString()		const { return liegeString; };
		CK2Title*				getLiege()				const { return liege; };
		vector<CK2Title*>		getVassals()			const { return vassals; };
		vector<CK2Title*>		getDeJureVassals()	const { return deJureVassals; };
		CK2Title*				getDeJureLiege()		const { return deJureLiege; };
		bool						isIndependent()		const { return independent; };
		bool						isInHRE()				const { return inHRE; };
		bool						isActive()				const { return active; };
		bool						isDynamic()				const { return dynamic; };
		EU3Country*				getDstCountry()		const { return dstCountry; };

		bool						eatTitle(CK2Title* target, bool checkInheritance);

		bool						hasUnionWith(CK2Title* other, bool& otherDominant /* out */) const;
		bool						hasRMWith(CK2Title* other) const;
		bool						hasAllianceWith(CK2Title* other) const;
		int						getRelationsWith(CK2Title* other) const;

	private:
		void								setDeJureLiege(CK2Title* _deJureLiege);
		void								addVassal(CK2Title*);
		void								removeVassal(CK2Title*);
		CK2Character*					getElectiveHeir(map<int, CK2Character*>&);
		CK2Character*					getTurkishSuccessionHeir();

		string							titleString;
		CK2Character*					holder;
		CK2Character*					heir;
		string							successionLaw;
		string							genderLaw;
		vector< pair<int, int> >	nominees;		// id, votes
		string							CA;
		vector<CK2History*>			history;
		string							liegeString;
		CK2Title*						liege;
		vector<CK2Title*>				vassals;
		vector<CK2Title*>				deJureVassals;
		string							deJureLiegeString;
		CK2Title*						deJureLiege;
		
		CK2Barony*							settlement;

		bool								independent;
		bool								inHRE;
		bool								active;
		bool								dynamic;

		EU3Country*						dstCountry;
};



#endif // CK2TITLE_H_