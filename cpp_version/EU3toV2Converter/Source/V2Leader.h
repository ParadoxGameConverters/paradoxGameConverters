#ifndef V2LEADER_H_
#define V2LEADER_H_



#include "date.h"
#include <string>
using namespace std;

class V2Country;



class V2Leader
{
	public:
		void	init(V2Country*);
		void	output(FILE* output);
		void	setName(string _name) { name = _name; };
		void	setActivationDate(date activation) { activationDate = activation; };
		void	setLand(bool land) { isLand = land; };
		void	setTraits(string _personality, string _background) { personality = _personality; background = _background; };
		int	getID() const { return id; };
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