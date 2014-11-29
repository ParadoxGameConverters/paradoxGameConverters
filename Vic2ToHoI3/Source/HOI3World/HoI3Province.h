/*Copyright (c) 2014 The Paradox Game Converters Project

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
		void determineColonial();
		void addCore(string);

		void				clearCores()								{ cores.clear(); };
		void				setCoastal(bool _coastal)				{ coastal = _coastal; };
		void				setName(string _name)					{ name = _name; };
		void				setOwner(string _owner)					{ owner = _owner; };
		void				setLandConnection(bool _connection)	{ landConnection = _connection; };
		void				setSameContinent(bool _same)			{ sameContinent = _same; };

		const V2Province*	getSrcProvince()		const { return srcProvince; };
		string					getOwner()				const { return owner; };
		int						getNum()					const { return num; };
		string					getName()				const { return name; };
		bool						isCoastal()				const { return coastal; };
		bool						hasNavalBase()			const { return (naval_base > 0); };

		void		requireNavalBase(int min);
		void		requireAirBase(int min);
		void		requireIndustry(int min);
		void		requireLandFort(int min);
		void		requireCoastalFort(int min);
		void		requireAntiAir(int min);
		void		requireInfrastructure(int min);

		void		setAvgMil(double mil) { avg_mil = mil; };

		void		setNCrafts(int nc);
		void setManpower(double newManpower) { manpower = newManpower; };
		void setLeadership(double newLeadership) { leadership = newLeadership; };
		bool isLand() const { return is_land; };

		int getAirBase() const { return air_base; };
		int getNavalBase() const { return naval_base; };
	private:
		void outputUnits(FILE*) const;

		const V2Province*			srcProvince;
		
		string							filename;
		bool								coastal;
		int								num;
		string							name;
		string							owner;
		vector<string>					cores;
		bool								landConnection;
		bool								sameContinent;
		string							terrain;
		bool			is_land;
		bool			is_blacklisted_port;

		double			avg_mil;
		int				ncrafts;

		int				points;
		double			metal;
		double			energy;
		double			oil;
		double			rare_materials;
		double			manpower;
		double			leadership;

		int				naval_base;
		int				air_base;
		int				industry;
		int				coastal_fort;
		int				land_fort;
		int				anti_air;
		int				infrastructure;
};



#endif // V2PROVINCE_H_