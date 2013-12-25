#ifndef HoI3PROVINCE_H_
#define HoI3PROVINCE_H_

#include <vector>
#include <string>

using namespace std;

class HoI3Province {
	public:
		void		init(int number);
		int			getNum();
		string		getName();
		void		setName(string);
		void		setOwner(string);
		string		getOwner();
		void		addCore(string);
		bool		isLand();
		void		setCoastal(bool);
		bool		isCoastal();
		bool		isBlacklistedPort();
		void		output(FILE*);

		double		getAvgMil() const { return avg_mil; };
		void		setAvgMil(double mil) { avg_mil = mil; };

		void		setNCrafts(int nc);

		void		requireNavalBase(int min);
		void		requireAirBase(int min);
		void		requireIndustry(int min);
		void		requireLandFort(int min);
		void		requireCoastalFort(int min);
		void		requireAntiAir(int min);
		void		requireInfrastructure(int min);

	private:
		int				num;
		string			name;
		string			owner;
		//controller
		vector<string>	cores;
		bool			is_land;
		bool			is_coastal;
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


#endif // HoI3PROVINCE_H_