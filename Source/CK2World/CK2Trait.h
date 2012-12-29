#ifndef CK2TRAIT_H_
#define CK2TRAIT_H_

#include <string>
#include <vector>
using namespace std;

class Object;

class CK2Trait
{
	public:
		CK2Trait(Object*);

		int	diplomacy;
		int	martial;
		int	stewardship;
		int	intrigue;
		int	learning;

		string name;
		vector<string> opposites;

		bool isOppositeOf(CK2Trait* other) const;

		// opinion values
		int vassal_opinion;
		int liege_opinion;
		int sex_appeal_opinion;
		int same_opinion;
		int opposite_opinion;
		int church_opinion;
		int same_opinion_if_same_religion;
		int twin_opinion;
		int spouse_opinion;
		int same_religion_opinion;
		int infidel_opinion;
		int muslim_opinion;
		int dynasty_opinion;
};



#endif // CK2TRAIT_H_