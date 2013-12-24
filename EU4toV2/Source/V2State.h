#ifndef V2Province_H_
#define V2Province_H_



#include "EU4World.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

class V2Province;
class V2Factory;
class EU4World;



class V2State
{
	public:
		V2State(int newId, V2Province* firstProvince);
		void output(FILE* output) const;

		void addRailroads();
		void setupPops(WorldType game, string primaryCulture, vector<string> acceptedCultures, string religion, double nationalConModifier, double nationalMilModifier);

		bool	isCoastal() const;
		bool	hasLocalSupply(string product) const;
		int	getCraftsmenPerFactory() const;
		bool	provInState(int id) const;

		void	addProvince(V2Province* newProvince) { provinces.push_back(newProvince); };
		void	addFactory(const V2Factory* factory) { factories.push_back(factory); };
		void	setColonial(bool isIt) { colonial = isIt; };

		bool						isColonial()		const { return colonial; };
		int						getFactoryCount()	const { return factories.size(); };
		int						getID()				const { return id; };
		vector<V2Province*>	getProvinces()		const { return provinces; };
	private:
		int	getStatePopulation() const;
		bool	hasCOT();
		int								id;
		bool								colonial;
		vector<V2Province*>			provinces;
		vector<const V2Factory*>	factories;
};


#endif	// V2Province_H_