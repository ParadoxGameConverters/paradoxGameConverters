#ifndef V2LEADER_H_
#define V2LEADER_H_



#include "date.h"
#include <string>
using namespace std;

class EU4Leader;
class V2Country;
class V2LeaderTraits;



class V2Leader
{
	public:
		V2Leader(string tag, const EU4Leader* oldLeader, const V2LeaderTraits& traits);
		void output(FILE* output) const;

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