#ifndef EU4ARMY_H_
#define EU4ARMY_H_



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
	heavy_ship,
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
	"heavy_ship",
	"light_ship",
	"galley",
	"transport"
};


typedef map<string, pair<RegimentCategory, int> > RegimentTypeMap;


class EU4Regiment // also Ship
{
	public:
		EU4Regiment(Object* obj);

		void						setCategory(const RegimentCategory cat) { category = cat; }
		void						setTypeStrength(const int typeStrength) { type_strength = typeStrength; }

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


class EU4Army // also Navy
{
	public:
		EU4Army(Object* obj);
		void					resolveRegimentTypes(const RegimentTypeMap& regimentTypeMap);
		double					getAverageStrength(RegimentCategory category) const;
		int						getTotalTypeStrength(RegimentCategory category) const;
		int						getProbabilisticHomeProvince(RegimentCategory category) const;
		void					blockHomeProvince(int home);

		string					getName() const { return name; }
		int						getLocation() const { return location; }
		int						getAtSea() const { return at_sea; }
		int						getLeaderID() const { return leaderID; }
	private:
		string					name;
		int						location;
		int						at_sea;
		vector<EU4Regiment*>	regiments;
		vector<int>				blocked_homes;
		int						leaderID;
};


void AddCategoryToRegimentTypeMap(Object* obj, RegimentCategory category, string categoryName, RegimentTypeMap& rtm);
void AddUnitFileToRegimentTypeMap(string directory, string name, RegimentTypeMap& rtm);



#endif