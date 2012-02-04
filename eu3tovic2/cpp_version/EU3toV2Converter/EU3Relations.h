#ifndef EU3RELATIONS_H_
#define EU3RELATIONS_H_


#include "Parsers/Object.h"
#include "Date.h"


class EU3Relations
{
	public:
		void				init(Object* obj);
		string				getCountry() const { return tag; };
		int					getRelations() const { return value; };
		bool				hasMilitaryAccess() const { return military_access; };
		date				getDiplomatLastSent() const { return last_send_diplomat; };
		date				getLastWar() const { return last_war; };
	private:
		string				tag;
		int					value;
		bool				military_access;
		date				last_send_diplomat;
		date				last_war;
};

#endif