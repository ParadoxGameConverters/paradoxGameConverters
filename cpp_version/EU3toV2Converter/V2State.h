#ifndef V2Province_H_
#define V2Province_H_


#include "V2Province.h"
#include "V2Factory.h"


class V2State
{
	public:
		V2State(int newId);
		void	addProvince(V2Province*);
		void	addFactory(V2Factory);
		void	setColonial(bool);
		bool	isColonial();
		void	output(FILE*);
		bool	isCoastal();
		bool	hasLocalSupply(string product);
		int		getCraftsmenPerFactory();
		int		getID();
		int		getFactoryCount();
		void	setupPops(EU3World& sourceWorld, string primaryCulture, vector<string> acceptedCultures, string religion, double nationalConModifier, double nationalMilModifier);
		int		getStatePopulation();
		bool		hasCOT();
	private:
		int						id;
		bool						colonial;
		vector<V2Province*>	provinces;
		vector<V2Factory>	factories;
};


#endif	// V2Province_H_