#ifndef EU3ARMY_H_
#define EU3ARMY_H_



#include <string>
#include <map>
#include <vector>
using namespace std;

class Object;



typedef enum
{
	// ground units
	infantry = 0,
	cavalry,
	artillery,
	// navy units
	big_ship,
	light_ship,
	galley,
	transport,
	// end of enum
	num_reg_categories
} RegimentCategory;


static const char* RegimentCategoryNames[] = {
	"infantry",
	"cavalry",
	"artillery",
	"big_ship",
	"light_ship",
	"galley",
	"transport"
};


typedef map<string, pair<RegimentCategory, int> > RegimentTypeMap;


class EU3Regiment // also Ship
{
	public:
		EU3Regiment(Object* obj);

		void						setCategory(RegimentCategory cat) { category = cat; }
		void						setTypeStrength(int typeStrength) { type_strength = typeStrength; }

		string					getName() const { return name; }
		string					getType() const { return type; }
		int						getHome() const { return home; }
		double					getStrength() const { return strength; }
		RegimentCategory		getCategory() const { return category; }
		int						getTypeStrength() const { return type_strength; }
	private:
		string					name;
		string					type;
		int						home;
		double					strength;
		RegimentCategory		category;
		int						type_strength;
};


class EU3Army // also Navy
{
	public:
		EU3Army(Object* obj);
		void						resolveRegimentTypes(const RegimentTypeMap& regimentTypeMap);
		double					getAverageStrength(RegimentCategory category);
		int						getTotalTypeStrength(RegimentCategory category);
		int						getProbabilisticHomeProvince(RegimentCategory category);
		void						blockHomeProvince(int home);

		string					getName() const { return name; }
		int						getLocation() const { return location; }
		int						getAtSea() const { return at_sea; }
		int						getLeaderID() const { return leaderID; }
	private:
		string					name;
		int						location;
		int						at_sea;
		vector<EU3Regiment*>	regiments;
		vector<int>				blocked_homes;
		int						leaderID;
};


void AddCategoryToRegimentTypeMap(Object* obj, RegimentCategory category, string categoryName, RegimentTypeMap& rtm);
void AddUnitFileToRegimentTypeMap(string directory, string name, RegimentTypeMap& rtm);



#endif