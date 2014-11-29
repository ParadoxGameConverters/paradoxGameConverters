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


#ifndef HOI3ARMY_H_
#define HOI3ARMY_H_

#include <io.h>
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

struct HoI3ArmyID
{
public:
	HoI3ArmyID();
	void output(FILE* out, int indentlevel) const;

	int id;
	int type;
};

enum ForceType
{
	land,
	navy,
	air
};

enum CommandLevel
{
	division,
	corps,
	army,
	armygroup,
	theatre
};

class HoI3RegimentType
{
	public:
								HoI3RegimentType() : force_type(land), max_strength(1), name("INVALID"), practicalBonus(""), practicalBonusFactor(0.0) { }; // req'd for map<>
								HoI3RegimentType(string type);
		ForceType				getForceType() const { return force_type; };
		string					getName() const { return name; };
		int						getMaxStrength() const { return max_strength; };

		const set<string>& getUsableBy() const { return usableBy; };
		string getPracticalBonus() const { return practicalBonus; };
		double getPracticalBonusFactor() const { return practicalBonusFactor; };

		bool operator< (const HoI3RegimentType& rhs) const { return this->name < rhs.name; }; // BE: I wanted to use a multiset, and it needs this
	private:
		ForceType				force_type;
		string					name;
		int						max_strength;
		set<string> usableBy;
		string practicalBonus;
		double practicalBonusFactor;
};

class HoI3Regiment // also Ship, Wing
{
	public:
		void					output(FILE* out, int indentlevel) const;
		void					setName(string _name) { name = _name; };
		void					setType(HoI3RegimentType _type) { type = _type; };
		void					setStrength(double str) { strength = str; };
		void					setOrganization(double org) { organization = org; };
		void					setExperience(double xp) { experience = xp; };
		ForceType				getForceType() const { return type.getForceType(); };
		void setHistoricalModel(unsigned _historicalModel) { historicalModel = _historicalModel; };
		void outputIntoProductionQueue(FILE* out) const;
		const HoI3RegimentType &getType() const { return type; };
	private:
		HoI3ArmyID				id;
		string					name;
		HoI3RegimentType		type;
		double					strength;
		double					organization;
		double					experience;
		unsigned historicalModel; // Tech level
};

class HoI3RegGroup // also Navy, Air
{
	public:
								HoI3RegGroup();
		void					output(FILE* out, int indentlevel = 0) const;
		void					setName(string _name) { name = _name; };
		void					setName();
		static void				resetRegGroupNameCounts();
		string					getName() const { return name; };
		void					setLocation(int provinceID) { location = provinceID; };
		void					setLocationContinent(string continent) { location_continent = continent; };
		string					getLocationContinent() const { return location_continent; };
		bool					addRegiment(HoI3Regiment reg, bool allowPromote);
		void					addChild(HoI3RegGroup group);
		void					setFuelSupplies(double supplyLevel) { supplies = supplyLevel; fuel = supplyLevel; };
		void					setForceType(ForceType _type) { force_type = _type; };
		ForceType				getForceType() const { return force_type; };
		void					setAtSea(int atSea) { at_sea = atSea; }
		void					setLeaderID(int id) { leaderID = id; };
		bool					isEmpty() const { return (regiments.size() == 0 && children.size() == 0); };
		static void				resetHQCounts();
		void					createHQs(HoI3RegimentType hqType);
		void					setCommandLevel(CommandLevel lvl) { command_level = lvl; };

		unsigned size() const;
		void setProductionQueue(const bool _productionQueue) { productionQueue = _productionQueue; };
		bool getProductionQueue() const { return productionQueue; };
		void outputIntoProductionQueue(FILE* out, const string& tag) const;
		void undoPracticalAddition(map<string, double>& practicals) const;
	private:
		HoI3RegGroup			createChild();

		HoI3ArmyID				id;
		string					name;
		int						location;
		string					location_continent; // cached value
		vector<HoI3Regiment>	regiments;
		double					supplies;
		double					fuel;
		ForceType				force_type;
		int						at_sea;
		int						leaderID;
		vector<HoI3RegGroup>	children;
		CommandLevel			command_level;
		bool productionQueue;
};

#endif