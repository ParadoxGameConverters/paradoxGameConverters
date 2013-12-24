#ifndef V2RELATIONS_H_
#define V2RELATIONS_H_



#include "Date.h"

class EU4Relations;



class V2Relations
{
	public:
		V2Relations(string newTag, EU4Relations* oldRelations);
		void output(FILE* out) const;

		void		setLevel(int level);

		string	getTag()			const { return tag; };
		int		getRelations()	const { return value; };
		int		getLevel()		const { return level; };
	private:
		string	tag;
		int		value;
		bool		militaryAccess;
		date		lastSendDiplomat;
		date		lastWar;
		int		level;
};



#endif