#ifndef V2Province_H_
#define V2Province_H_



#include "EU3World.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

class V2Province;
class V2Factory;
class EU3World;



class V2State
{
	public:
		V2State(int newId);
		void						addProvince(V2Province*);
		void						addFactory(V2Factory*);
		void						addRailroads();
		void						setColonial(bool);
		bool						isColonial();
		void						output(FILE*);
		bool						isCoastal();
		bool						hasLocalSupply(string product);
		int						getCraftsmenPerFactory();
		int						getID();
		int						getFactoryCount();
		void						setupPops(WorldType game, string primaryCulture, vector<string> acceptedCultures, string religion, double nationalConModifier, double nationalMilModifier);
		int						getStatePopulation();
		bool						hasCOT();
		bool						provInState(int id);
		vector<V2Province*>	getProvinces();
	private:
		int						id;
		bool						colonial;
		vector<V2Province*>	provinces;
		vector<V2Factory*>	factories;
};


#endif	// V2Province_H_