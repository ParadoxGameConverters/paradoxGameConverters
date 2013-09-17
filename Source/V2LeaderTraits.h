#ifndef LEADERTRAITS_H_
#define LEADERTRAITS_H_



#include <string>
#include <vector>
using namespace std;

class Object;



struct V2TraitConversion
{
	V2TraitConversion(Object* obj);
	bool matches(int fire, int shock, int manuever, int siege) const;

	int		req_fire;
	int		req_shock;
	int		req_manuever;
	int		req_siege;
	int		req_other;
	string	trait;
};


class V2LeaderTraits
{
	public:
		V2LeaderTraits();

		string	getPersonality(int fire, int shock, int manuever, int siege) const;
		string	getBackground(int fire, int shock, int manuever, int siege) const;
	private:
		vector<V2TraitConversion> personalities;
		vector<V2TraitConversion> backgrounds;
};



#endif // LEADERTRAITS_H_