/*Copyright(c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */


#ifndef EU4LEADER_H_
#define EU4LEADER_H_


#include "../date.h"

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
		string	name;					// the leader's name
		int		fire;					// the fire ability
		int		shock;				// the shock ability
		int		manuever;			// the manuever ability
		int		siege;				// the seige ability
		int		id;					// the id number
		string	type;					// the type of leader
		date		activationDate;	// the date this leader was activated
};



#endif // EU4LEADER_H_