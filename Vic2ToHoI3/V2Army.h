#ifndef V2ARMY_H_
#define V2ARMY_H_

#include "Parsers/Object.h"

class V2Regiment // also Ship
{
	public:
		void					init(Object* obj);
		string					getName() const { return name; };
		string					getType() const { return type; };
		double					getStrength() const { return strength; };
		double					getOrganization() const { return organization; };
		double					getExperience() const { return experience; };
		bool					getShip() const { return isShip; };

	private:
		string					name;
		string					type;
		double					strength;
		double					organization;
		double					experience;
		bool					isShip;
};

class V2Army // also Navy
{
	public:
		void					init(Object* obj);
		string					getName() const { return name; };
		int						getLocation() const { return location; };
		vector<V2Regiment>		getRegiments() const { return regiments; };
		bool					getNavy() const { return isNavy; };
		double					getSupplies() const { return supplies; };
		int						getAtSea() const { return at_sea; };
		int						getLeaderID() const { return leaderID; };

	private:
		string					name;
		int						location;
		vector<V2Regiment>		regiments;
		bool					isNavy;
		double					supplies;
		int						at_sea;
		int						leaderID;
};

#endif