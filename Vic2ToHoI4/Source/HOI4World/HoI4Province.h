/*Copyright (c) 2017 The Paradox Game Converters Project

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



#ifndef HoI4PROVINCE_H_
#define HoI4PROVINCE_H_



#include "../Configuration.h"
#include "../V2World/World.h"
#include "../V2World/Country.h"



class HoI4Province
{
	public:
		HoI4Province(const std::string& Owner, int State);
		void output() const;
		void convertFromOldProvince(const Vic2::Province* oldProvince);
		void addCore(const std::string& core);
		void addFilename(const std::string& _filename);

		void clearCores()										{ cores.clear(); }
		void setCoastal(bool _coastal)					{ coastal = _coastal; }
		void setName(const std::string& _name)							{ name = _name; }
		void setOwner(const std::string& _owner)						{ owner = _owner; }
		void addManpower(double newManpower)			{ manpower += newManpower; }
		void setManpower(double newManpower)			{ manpower = newManpower; }
		void addLeadership(double newLeadership)		{ leadership += newLeadership; }
		void setLeadership(double newLeadership)		{ leadership = newLeadership; }
		void addRawIndustry(double newRawIndustry)	{ rawIndustry += newRawIndustry; }
		void setRawIndustry(double newRawIndustry)	{ rawIndustry = newRawIndustry; }
		void setActualIndustry(int newIndustry)		{ industry = newIndustry; }
		void setPoints(int newPoints)						{ points = newPoints; }
		void addPoints(int newPoints)						{ points += newPoints; }
		void setnum(int number)						{ num = number; }
		std::string	getOwner()				const { return owner; }
		int		getNum()					const { return num; }
		std::string	getName()				const { return name; }
		bool		hasNavalBase()			const { return (naval_base > 0); }
		bool		isLand()					const { return is_land; }
		int		getAirBase()			const { return air_base; }
		int		getNavalBase()			const { return naval_base; }
		double	getManpower()			const { return manpower; }
		double	getLeadership()		const { return leadership; }
		double	getRawIndustry()		const { return rawIndustry; }
		int		getActualIndustry()	const { return industry; }
		
		std::vector<std::string>	getCores() const	{ return cores; }

		void		requireNavalBase(int min);
		void		requireAirBase(int min);
		void		requireIndustry(int min);
		void		requireInfrastructure(int min);

	private:
		HoI4Province(const HoI4Province&) = delete;
		HoI4Province& operator=(const HoI4Province&) = delete;

		std::map<std::string, std::string>	filenames;
		bool						coastal;
		int						num;
		std::string					name;
		std::string					owner;
		std::vector<std::string>			cores;
		bool						is_land;

		int				points;
		double			metal;
		double			energy;
		double			oil;
		double			rare_materials;
		double			manpower;
		double			leadership;
		double			rawIndustry;
		int				state;
		int				naval_base;
		int				air_base;
		int				industry;
		int				infrastructure;
};



#endif // V2PROVINCE_H_