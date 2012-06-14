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
		void					output(FILE* out);
		void					setName(string _name) { name = _name; };
		void					setPopID(int newPop) { popID = newPop; };
		void					setStrength(double str) { strength = str; };
		bool					getShip() const { return isShip; };
		RegimentCategory	getCategory() const { return category; };
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
								V2Army();
		void					output(FILE* out);
		void					addRegiment(V2Regiment reg);
		void					setName(string _name) { name = _name; };
		string				getName() const { return name; };
		void					setLocation(int provinceID) { location = provinceID; };
		void					setNavy(bool navy) { isNavy = navy; };
		bool					getNavy() const { return isNavy; };
		void					setArmyRemainders(RegimentCategory category, double remainder) { army_remainders[category] = remainder; };
		double				getArmyRemainder(RegimentCategory category) const { return army_remainders[category]; };
		void					setSourceArmy(EU3Army* source) { sourceArmy = source; };
		EU3Army*				getSourceArmy() const { return sourceArmy; };
		void					setAtSea(int atSea) { at_sea = atSea; }
		void					getRegimentCounts(int counts[num_reg_categories]);
		void					setLeaderID(int id) { leaderID = id; };
	private:
		V2ArmyID					id;
		string					name;
		int						location;
		vector<V2Regiment>	regiments;
		double					army_remainders[num_reg_categories];
		EU3Army*					sourceArmy; // only valid during conversion
		bool						isNavy;
		int						at_sea;
		int						leaderID;
};



#endif