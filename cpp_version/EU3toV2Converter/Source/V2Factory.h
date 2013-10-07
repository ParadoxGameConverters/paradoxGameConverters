#ifndef V2FACTORY_H_
#define V2FACTORY_H_



#include "V2Inventions.h"
#include <deque>
#include <vector>
#include <map>
using namespace std;

class Object;



struct V2FactoryType
{
	V2FactoryType(Object* factory);

	string						name;
	bool							requireCoastal;
	string						requireTech;
	vanillaInventionType		vanillaRequiredInvention;
	HODInventionType			HODRequiredInvention;
	bool							requireLocalInput;
	map<string,float>			inputs;
};


class V2Factory
{
	public:
		V2Factory(const V2FactoryType* _type) : type(_type) {};
		void					output(FILE* output) const;
		map<string,float>	getRequiredRGO() const;

		bool						requiresCoastal()					const { return type->requireCoastal; }
		string					getRequiredTech()					const { return type->requireTech; }
		vanillaInventionType	getVanillaRequiredInvention()	const { return type->vanillaRequiredInvention; }
		HODInventionType		getHODRequiredInvention()		const { return type->HODRequiredInvention; }
		string					getTypeName()						const { return type->name; }
	private:
		const V2FactoryType* type;
};


class V2FactoryFactory
{
	public:
		V2FactoryFactory(string V2loc);
		deque<V2Factory*>	buildFactories() const;
	private:
		void					loadRequiredTechs(string filename);
		void					loadRequiredInventions(string filename);
		vector<pair<V2FactoryType*, int>>	factoryCounts;
		map<string, V2FactoryType*>			factoryTypes;
		map<string, string>						factoryTechReqs;
		map<string, string>						factoryInventionReqs;
};



#endif // V2FACTORY_H_