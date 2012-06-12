#ifndef LEADERTRAITS_H_
#define LEADERTRAITS_H_

#include "Parsers/Object.h"

struct TraitConversion
{
	void	init(Object* obj);
	bool	matches(int fire, int shock, int manuever, int siege);

	int		req_fire;
	int		req_shock;
	int		req_manuever;
	int		req_siege;
	int		req_other;
	string	trait;
};

class LeaderTraits
{
public:
	void	init();
	string	getPersonality(int fire, int shock, int manuever, int siege);
	string	getBackground(int fire, int shock, int manuever, int siege);

private:
	vector<TraitConversion> personalities;
	vector<TraitConversion> backgrounds;
};

#endif // LEADERTRAITS_H_