#ifndef EU4LEADER_H_
#define EU4LEADER_H_


#include "date.h"

class Object;



class EU4Leader
{
	public:
		EU4Leader(Object* obj);
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



#endif // EU4LEADER_H_