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

class CK2Title
{
	public:
		CK2Title(string _titleString);
		void	init(Object*, map<int, CK2Character*>&);

		void						addLiege(CK2Title*);
		void						addToHRE();
		void						determineHeir(map<int, CK2Character*>&);
		void						setHeir(CK2Character*);
		void						setDeJureLiege(const map<string, CK2Title*>& titles);
		void						addDeJureVassals(vector<Object*>, map<string, CK2Title*>& titles, CK2World* world);

		void						getCultureWeights(map<string, int>& cultureWeights, const cultureMapping& cultureMap) const;

		string					getTitleString()		const { return titleString; };
		CK2Character*			getHolder()				const { return holder; };
		CK2Character*			getHeir()				const { return heir; };
		string					getSuccessionLaw()	const { return successionLaw; };
		vector<CK2History*>	getHistory()			const { return history; };
		string					getLiegeString()		const { return liegeString; };
		CK2Title*				getLiege()				const { return liege; };
		CK2Title*				getDeJureLiege()		const { return deJureLiege; };
		bool						isIndependent()		const { return independent; };
		bool						isInHRE()				const { return inHRE; };
	private:
		void								setDeJureLiege(CK2Title* _deJureLiege) { deJureLiege = _deJureLiege; }
		void								addVassal(CK2Title*);
		CK2Character*					getFeudalElectiveHeir(map<int, CK2Character*>&);
		CK2Character*					getTurkishSuccessionHeir();

		string							titleString;
		CK2Character*					holder;
		CK2Character*					heir;
		string							successionLaw;
		string							genderLaw;
		vector< pair<int, int> >	nominees;		// id, votes
		vector<CK2History*>			history;
		string							liegeString;
		CK2Title*						liege;
		vector<CK2Title*>				vassals;
		string							deJureLiegeString;
		CK2Title*						deJureLiege;

		bool								independent;
		bool								inHRE;
};



#endif // CK2TITLE_H_