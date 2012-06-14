#ifndef V2RELATIONS_H_
#define V2RELATIONS_H_



#include "Date.h"



class V2Relations
{
	public:
		void		init(string newTag);
		void		output(FILE* out);
		int		getRelations() const { return value; };
		void 		setRelations(int newRel) { value = newRel; };
		void		setMilitaryAccess(bool hasAccess) { military_access = hasAccess; };
		void		setDiplomatLastSent(date lastSent) { last_send_diplomat = lastSent; };
		void		setLastWar(date lastWar) { last_war = lastWar; };
		int		getLevel() const { return level; };
		void		setLevel(int level);
		string	getTag() const { return tag; };
	private:
		string	tag;
		int		value;
		bool		military_access;
		date		last_send_diplomat;
		date		last_war;
		int		level;
};



#endif