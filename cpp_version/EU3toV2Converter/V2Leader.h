#ifndef V2LEADER_H_
#define V2LEADER_H_

#include <string>
using namespace std;
#include "date.h"

class V2Country; // forward decl to break circular ref

class V2Leader
{
public:
	void	init(V2Country*);
	void	output(FILE* output);
	void	setName(string _name) { name = _name; };
	void	setActivationDate(date activation) { activationDate = activation; };
	void	setLand(bool land) { isLand = land; };
	void	setTraits(string _personality, string _background) { personality = _personality; background = _background; };
	int		getID() const { return id; };

private:
	string	name;
	date	activationDate;
	bool	isLand;
	string	personality;
	string	background;

	string	country;
	string	picture;
	int		id;
};

#endif // V2LEADER_H_