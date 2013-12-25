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



#ifndef EU3ARMY_H_
#define EU3ARMY_H_


#include "..\Mappers.h"
#include <fstream>
#include <map>
#include <vector>
using namespace std;


class EU3Province;
class CK2Subunit;
class CK2Army;


class EU3Regiment
{
	public:
		EU3Regiment(const string _type, const double _strength);
		void	output(FILE*);

		void	setHomeProvince(int _home)		{ home = _home; };
		void	setName(string _name)			{ name = _name; };

		string	getType()	const { return type; };
	private:
		int						id;
		string					name;
		int						home;
		string					type;
		double					morale;
		double					strength;
};


class EU3Army // also Navy
{
	public:
		EU3Army(const CK2Army* srcArmy, const inverseProvinceMapping inverseProvinceMap, const string infantryType, const string cavalryType, map<int, EU3Province*> provinces, double& manpower);
		void		output(FILE*);

		int	getNumRegiments()	const { return regiments.size(); };
		int	getNumInfantry()	const;
	private:
		int						id;
		string					name;
		//int						previous;
		double					movementProgress;
		vector<int>				path;
		int						location;	
		vector<EU3Regiment*>	regiments;
		//int						stagingProvince;
		//int						base;
		//int						at_sea;

		//int						leaderID;
};


#endif // EU3ARMY_H_