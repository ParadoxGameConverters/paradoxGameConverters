#ifndef V2ARMY_H_
#define V2ARMY_H_



#include "EU3Army.h"



struct V2ArmyID
{
	public:
		V2ArmyID();
		void output(FILE* out, int indentlevel);

		int id;
		int type;
};


class V2Regiment // also Ship
{
	public:
		V2Regiment(RegimentCategory rc);
		void output(FILE* out);

		void setName(string _name)		{ name = _name; };
		void setPopID(int newPop)		{ popID = newPop; };
		void setStrength(double str)	{ strength = str; };

		bool					getShip()		const { return isShip; };
		RegimentCategory	getCategory()	const { return category; };
	private:
		V2ArmyID				id;
		string				name;
		string				type;
		int					popID;
		double				strength;
		bool					isShip;
		RegimentCategory	category;
};


class V2Army // also Navy
{
	public:
		V2Army(EU3Army* oldArmy, map<int, int> leaderIDMap);
		void					output(FILE* out);
		void					addRegiment(V2Regiment reg);

		void					setLocation(int provinceID)												{ location = provinceID; };
		void					setNavy(bool navy)															{ isNavy = navy; };
		void					setArmyRemainders(RegimentCategory category, double remainder)	{ armyRemainders[category] = remainder; };

		string				getName() const { return name; };
		void					getRegimentCounts(int counts[num_reg_categories]);
		double				getArmyRemainder(RegimentCategory category) const { return armyRemainders[category]; };
		EU3Army*				getSourceArmy() const { return sourceArmy; };
		bool					getNavy() const { return isNavy; };
	private:
		V2ArmyID					id;
		string					name;
		int						location;
		vector<V2Regiment>	regiments;
		double					armyRemainders[num_reg_categories];
		EU3Army*					sourceArmy; // only valid during conversion
		int						at_sea;
		int						leaderID;
		bool						isNavy;
};



#endif