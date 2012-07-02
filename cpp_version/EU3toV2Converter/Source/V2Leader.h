#ifndef V2LEADER_H_
#define V2LEADER_H_



#include "date.h"
#include <string>
using namespace std;

class EU3Leader;
class V2Country;
class V2LeaderTraits;



class V2Leader
{
	public:
		V2Leader(const V2Country* _country, const EU3Leader* oldLeader, const V2LeaderTraits& traits);
		void output(FILE* output);

		int getID() const { return id; };
	private:
		string	name;
		date		activationDate;
		bool		isLand;
		string	personality;
		string	background;
		string	country;
		string	picture;
		int		id;
};



#endif // V2LEADER_H_