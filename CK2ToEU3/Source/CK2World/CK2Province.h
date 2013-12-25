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



#ifndef CK2PROVINCE_H_
#define CK2PROVINCE_H_


#include "..\Mappers.h"
#include <vector>
#include <map>
using namespace std;



class Object;
class CK2Title;
class CK2BuildingFactory;
class CK2Barony;
class CK2Religion;
class CK2Character;
class CK2Version;

class CK2Province
{
	public:
		CK2Province(Object*, map<string, CK2Title*>& titles, map<int, CK2Character*>& characters, const CK2BuildingFactory* buildingFactory, CK2Version& version);

		int						getNumber()				const { return number; };
		vector<CK2Barony*>	getBaronies()			const { return baronies; };
		string					getCulture()			const { return culture; };
		CK2Religion*			getReligion()			const { return religion; };
		vector<double>			getTechLevels()		const { return techLevels; } ;
		bool						hasTradePost()			const { return tradePost; };
		int						getTpGarrisonSize()	const { return tpGarrisonSize; };
		CK2Title*				getTPOwner()			const { return tpOwner; };
	private:
		int						number;
		bool						tradePost;
		int						tpGarrisonSize;
		CK2Title*				tpOwner;
		vector<CK2Barony*>	baronies;
		string					culture;
		CK2Religion*			religion;
		vector<double>			techLevels;
};



#endif // CK2PROVINCE_H_