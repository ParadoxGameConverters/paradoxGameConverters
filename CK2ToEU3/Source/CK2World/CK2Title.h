/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



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
		CK2Title(string _titleString, int* color);
		void	init(Object*, map<int, CK2Character*>&, const CK2BuildingFactory* buildingFactory);

		void						setLiege(CK2Title*);
		void						addToHRE();
		void						determineHeir(map<int, CK2Character*>&);
		void						setHeir(CK2Character*);
		void						setSuccessionLaw(string _successionLaw);
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
		string					getGenderLaw()			const { return genderLaw; };
		string					getCA()					const { return CA; };
		int						getFeudalContract()	const { return feudalContract; };
		int						getTempleContract()	const { return templeContract; };
		int						getCityContract()		const { return cityContract; };
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
		const int*				getColor()				const { return color; };
		EU3Country*				getDstCountry()		const { return dstCountry; };

		bool						eatTitle(CK2Title* target, bool checkInheritance);

		bool						hasUnionWith(CK2Title* other, bool& otherDominant /* out */) const;
		bool						hasRMWith(CK2Title* other) const;
		bool						hasAllianceWith(CK2Title* other) const;
		int						getRelationsWith(CK2Title* other, CK2Version& version) const;

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
		int								feudalContract;
		int								templeContract;
		int								cityContract;
		vector<CK2History*>			history;
		string							liegeString;
		CK2Title*						liege;
		vector<CK2Title*>				vassals;
		vector<CK2Title*>				deJureVassals;
		string							deJureLiegeString;
		CK2Title*						deJureLiege;
		
		CK2Barony*						settlement;

		bool								independent;
		bool								inHRE;
		bool								active;
		bool								dynamic;

		int								color[3];

		EU3Country*						dstCountry;
};



#endif // CK2TITLE_H_