#ifndef EU3LEADER_H_
#define EU3LEADER_H_


#include "date.h"

class Object;



class EU3Leader
{
	public:
		EU3Leader();
		void		init(Object* obj);
		string	getName() const { return name; };
		int		getFire() const { return fire; };
		int		getShock() const { return shock; };
		int		getManuever() const { return manuever; };
		int		getSiege() const { return siege; };
		int		getID() const { return id; };
		bool		isLand() const;
		date		getActivationDate() const { return activationDate; }
	private:
		string	name;
		int		fire;
		int		shock;
		int		manuever;
		int		siege;
		int		id;
		string	type;
		date		activationDate;
};



#endif // EU3LEADER_H_