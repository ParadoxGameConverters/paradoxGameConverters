#ifndef HOI3ARMY_H_
#define HOI3ARMY_H_

#include <io.h>
#include <string>
#include <vector>

using namespace std;

struct HoI3ArmyID
{
public:
	HoI3ArmyID();
	void output(FILE* out, int indentlevel);

	int id;
	int type;
};

enum ForceType
{
	land,
	navy,
	air
};

class HoI3RegimentType
{
	public:
								HoI3RegimentType() : force_type(land), max_strength(1), name("INVALID") { }; // req'd for map<>
								HoI3RegimentType(string type);
		ForceType				getForceType() const { return force_type; };
		string					getName() { return name; };
		int						getMaxStrength() { return max_strength; };

	private:
		ForceType				force_type;
		string					name;
		int						max_strength;
};

class HoI3Regiment // also Ship, Wing
{
	public:
		void					output(FILE* out);
		void					setName(string _name) { name = _name; };
		void					setType(HoI3RegimentType _type) { type = _type; };
		void					setStrength(double str) { strength = str; };
		void					setOrganization(double org) { organization = org; };
		void					setExperience(double xp) { experience = xp; };
		ForceType				getForceType() const { return type.getForceType(); };
	private:
		HoI3ArmyID				id;
		string					name;
		HoI3RegimentType		type;
		double					strength;
		double					organization;
		double					experience;
};

class HoI3Army // really Division; also Navy, Air
{
	public:
								HoI3Army();
		void					output(FILE* out);
		void					setName(string _name) { name = _name; };
		string					getName() const { return name; };
		void					setLocation(int provinceID) { location = provinceID; };
		void					addRegiment(HoI3Regiment reg);
		void					setFuelSupplies(double supplyLevel) { supplies = supplyLevel; fuel = supplyLevel; };
		void					setForceType(ForceType _type) { force_type = _type; };
		ForceType				getForceType() const { return force_type; };
		void					setAtSea(int atSea) { at_sea = atSea; }
		void					setLeaderID(int id) { leaderID = id; };
		bool					isEmpty() { return (regiments.size() == 0); };
	private:
		HoI3ArmyID				id;
		string					name;
		int						location;
		vector<HoI3Regiment>	regiments;
		double					supplies;
		double					fuel;
		ForceType				force_type;
		int						at_sea;
		int						leaderID;
};

#endif