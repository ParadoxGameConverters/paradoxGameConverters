/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#ifndef EU3NAVY_H_
#define EU3NAVY_H_


#include "..\Mappers.h"
#include <string>
using namespace std;



class CK2Subunit;
class CK2Army;
class EU3Province;
class EU3Army;



class EU3Ship
{
	public:
		EU3Ship(const string _type, const double _strength);
		void	output(FILE*);

		void	setHomeProvince(int _home)		{ home = _home; };
		void	setName(string _name)			{ name = _name; };
	private:
		int						id;
		string					name;
		int						home;
		string					type;
		double					morale;
		double					strength;
};


class EU3Navy // also Navy
{
	public:
		EU3Navy(const CK2Army* srcNavy, const inverseProvinceMapping inverseProvinceMap, const string transportType, const string infantryType, const string cavalryType, map<int, EU3Province*> provinces, double& manpower);
		EU3Navy(int numNavies, int numShips, int provNum, const string transportType, map<int, EU3Province*> provinces);

		int	getNumShips()	const { return ships.size(); };
		int	getLocation()	const { return location; };

		void		output(FILE*);
	private:
		int						id;
		string					name;
		double					movementProgress;
		vector<int>				path;
		int						location;	
		vector<EU3Ship*>		ships;
		bool						atSea;

		vector<EU3Army*>		transportedArmies;
};


#endif	// EU3NAVY_H_
