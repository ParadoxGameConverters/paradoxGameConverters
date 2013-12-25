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
		void					output(FILE* out, int indentlevel);
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

class HoI3RegGroup // also Navy, Air
{
	public:
								HoI3RegGroup();
		void					output(FILE* out, int indentlevel);
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
		bool					isEmpty() { return (regiments.size() == 0 && children.size() == 0); };
		static void				resetHQCounts();
		void					createHQs(HoI3RegimentType hqType);
		void					setCommandLevel(CommandLevel lvl) { command_level = lvl; };

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
};

#endif