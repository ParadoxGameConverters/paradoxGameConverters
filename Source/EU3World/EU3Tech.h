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



#ifndef EU3TECH_H_
#define EU3TECH_H_



#include "..\Parsers\Object.h"
#include "..\Date.h"



class EU3Tech
{
	public:
		EU3Tech(date startDate, Object* mainObj, Object* governmentObj, Object* productionObj, Object* tradeObj, Object* navalObj, Object* landObj);

		double	getGovernmentTech(string techGroup) const;
		double	getProductionTech(string techGroup) const;
		double	getTradeTech(string techGroup) const;
		double	getNavalTech(string techGroup) const;
		double	getLandTech(string techGroup) const;

		double	getGovernmentBaseCost(date startDate, int level) const;
		double	getProductionBaseCost(date startDate, int level) const;
		double	getTradeBaseCost(date startDate, int level) const;
		double	getNavalBaseCost(date startDate, int level) const;
		double	getLandBaseCost(date startDate, int level) const;

		double	getGroupModifier(string group) const;
	private:
		map<string, pair<int, double> >	groups; // name, start level, modifier
		map<string, vector<double> >		startingTechLevels; // tech group, tech levels
		vector<int>								governmentYears;
		vector<int>								productionYears;
		vector<int>								tradeYears;
		vector<int>								navalYears;
		vector<int>								landYears;
};



#endif // EU3TECH_H_