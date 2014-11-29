/*Copyright (c) 2014 The Paradox Game Converters Project

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



#ifndef V2ARMY_H_
#define V2ARMY_H_



#include "../Eu4World/EU4Army.h"



struct V2ArmyID
{
	public:
		V2ArmyID();
		void output(FILE* out, int indentlevel) const;

		int id;
		int type;
};


class V2Regiment // also Ship
{
	public:
		V2Regiment(RegimentCategory rc);
		V2Regiment(Object* obj);
		void output(FILE* out) const;

		void setName(string _name)		{ name = _name; };
		void setPopID(int newPop)		{ popID = newPop; };
		void setStrength(double str)	{ strength = str; };

		bool					getShip()		const { return isShip; };
		RegimentCategory	getCategory()	const { return category; };
		string getName() const { return name; };
		string getType() const { return type; };
		double getStrength() const { return strength; };
		double getOrganization() const { return organization; };
		double getExperience() const { return experience; };
	private:
		V2ArmyID				id;
		string				name;
		string				type;
		int					popID;
		double				strength;
		bool					isShip;
		RegimentCategory	category;

		double					organization;
		double					experience;
};


class V2Army // also Navy
{
	public:
		V2Army(EU4Army* oldArmy, map<int, int> leaderIDMap);
		V2Army(Object* obj);
		void					output(FILE* out) const;
		void					addRegiment(V2Regiment reg);

		void					setLocation(int provinceID)												{ location = provinceID; };
		void					setNavy(bool navy)															{ isNavy = navy; };
		void					setArmyRemainders(RegimentCategory category, double remainder)	{ armyRemainders[category] = remainder; };

		string				getName() const { return name; };
		void					getRegimentCounts(int counts[num_reg_categories]) const;
		double				getArmyRemainder(RegimentCategory category) const { return armyRemainders[category]; };
		EU4Army*				getSourceArmy() const { return sourceArmy; };
		bool					getNavy() const { return isNavy; };
		double getSupplies() const { return supplies; };
		int getAtSea() const { return at_sea; };
		int getLocation() const { return location; };
		vector<V2Regiment> getRegiments() const { return regiments; };
	private:
		V2ArmyID					id;
		string					name;
		int						location;
		vector<V2Regiment>	regiments;
		double					armyRemainders[num_reg_categories];
		EU4Army*					sourceArmy; // only valid during conversion
		double					supplies;
		int						at_sea;
		int						leaderID;
		bool						isNavy;
};



#endif