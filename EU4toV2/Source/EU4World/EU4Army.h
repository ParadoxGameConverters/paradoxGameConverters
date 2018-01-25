/*Copyright(c) 2017 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */



#ifndef EU4ARMY_H_
#define EU4ARMY_H_



#include <string>
#include <map>
#include <memory>
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


typedef map<string, pair<RegimentCategory, int> > RegimentTypeMap;	// regiment category, regiment category, required strength


class EU4Regiment // also Ship
{
	public:
		EU4Regiment(shared_ptr<Object> obj);

		void						setCategory(const RegimentCategory cat) { category = cat; }
		void						setTypeStrength(const int typeStrength) { type_strength = typeStrength; }

		string					getName() const { return name; }
		string					getType() const { return type; }
		int						getHome() const { return home; }
		double					getStrength() const { return strength; }
		RegimentCategory		getCategory() const { return category; }
		int						getTypeStrength() const { return type_strength; }
	private:
		string					name;				// the name of the regiment
		string					type;				// the type of regiment
		int						home;				// the home province of the regiment
		double					strength;		// the strength of the regiment
		RegimentCategory		category;		// the category of the regiment
		int						type_strength;	// the strength of this regiment type
};


class EU4Army // also Navy
{
	public:
		EU4Army(shared_ptr<Object> obj);
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
		string					name;				// the name of the army
		int						location;		// the location of the army
		int						at_sea;			// whether or not this army (or navy) is at sea
		vector<EU4Regiment*>	regiments;		// the regiments in this army
		vector<int>				blocked_homes;	// invalid homes for this army
		int						leaderID;		// the id of the leader for this army (if any)
};


void AddCategoryToRegimentTypeMap(shared_ptr<Object> obj, RegimentCategory category, string categoryName, RegimentTypeMap& rtm);
void AddUnitFileToRegimentTypeMap(string directory, string name, RegimentTypeMap& rtm);



#endif