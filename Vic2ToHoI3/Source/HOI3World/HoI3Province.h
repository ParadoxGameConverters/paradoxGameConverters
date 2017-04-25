/*Copyright (c) 2016 The Paradox Game Converters Project

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



#ifndef HoI3PROVINCE_H_
#define HoI3PROVINCE_H_



#include "../Configuration.h"
#include "../V2World/V2World.h"
#include "../V2World/V2Country.h"



class HoI3Province
{
	public:
		HoI3Province(string _filename);
		void output() const;
		void convertFromOldProvince(const V2Province* oldProvince);
		void addCore(string);
		void addFilename(string _filename);

		void clearCores()										{ cores.clear(); }
		void setCoastal(bool _coastal)					{ coastal = _coastal; }
		void setName(string _name)							{ name = _name; }
		void setOwner(string _owner)						{ owner = _owner; }
		void addManpower(double newManpower)			{ manpower += newManpower; }
		void setManpower(double newManpower)			{ manpower = newManpower; }
		void addLeadership(double newLeadership)		{ leadership += newLeadership; }
		void setLeadership(double newLeadership)		{ leadership = newLeadership; }
		void addRawIndustry(double newRawIndustry)	{ rawIndustry += newRawIndustry; }
		void setRawIndustry(double newRawIndustry)	{ rawIndustry = newRawIndustry; }
		void setActualIndustry(int newIndustry)		{ industry = newIndustry; }
		void setPoints(int newPoints)						{ points = newPoints; }
		void addPoints(int newPoints)						{ points += newPoints; }

		string	getOwner()				const { return owner; }
		int		getNum()					const { return num; }
		string	getName()				const { return name; }
		bool		hasNavalBase()			const { return (naval_base > 0); }
		bool		isLand()					const { return is_land; }
		int		getAirBase()			const { return air_base; }
		int		getNavalBase()			const { return naval_base; }
		double	getManpower()			const { return manpower; }
		double	getLeadership()		const { return leadership; }
		double	getRawIndustry()		const { return rawIndustry; }
		int		getActualIndustry()	const { return industry; }
		
		vector<string>	getCores() const	{ return cores; }

		void		requireNavalBase(int min);
		void		requireAirBase(int min);
		void		requireIndustry(int min);
		void		requireLandFort(int min);
		void		requireCoastalFort(int min);
		void		requireInfrastructure(int min);

	private:
		map<string, string>	filenames;
		bool						coastal;
		int						num;
		string					name;
		string					owner;
		vector<string>			cores;
		bool						is_land;

		int				ncrafts;

		int				points;
		double			metal;
		double			energy;
		double			oil;
		double			rare_materials;
		double			manpower;
		double			leadership;
		double			rawIndustry;

		int				naval_base;
		int				air_base;
		int				industry;
		int				coastal_fort;
		int				land_fort;
		int				infrastructure;
};



#endif // V2PROVINCE_H_