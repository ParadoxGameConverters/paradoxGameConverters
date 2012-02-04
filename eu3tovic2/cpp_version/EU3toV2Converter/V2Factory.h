#ifndef V2FACTORY_H_
#define V2FACTORY_H_

#include "Parsers/Object.h"
#include "V2Inventions.h"
#include <deque>

struct V2FactoryType
{
	void	init(Object* factory);

	string				name;
	bool				requireCoastal;
	string				requireTech;
	inventionType		requireInvention;
	bool				requireLocalInput;
	map<string,float>	inputs;
};

class V2Factory
{
public:
	V2Factory(const V2FactoryType* _type) : type(_type) {};

	bool			requiresCoastal() const { return type->requireCoastal; }
	string			getRequiredTech() const { return type->requireTech; }
	inventionType	getRequiredInvention() const { return type->requireInvention; }
	vector<string>	getRequiredRGO() const;
	string			getTypeName() const { return type->name; }

	void			output(FILE* output);

private:
	const V2FactoryType* type;
};

class V2FactoryFactory
{
public:
	void					init(string V2loc);
	deque<V2Factory>		buildFactories();

private:
	void					loadRequiredTechs(string filename);
	void					loadRequiredInventions(string filename);

	vector<pair<V2FactoryType*, int>>	factoryCounts;
	map<string, V2FactoryType>			factoryTypes;
	map<string, string>					factoryTechReqs;
	map<string, string>					factoryInventionReqs;
};

#endif // V2FACTORY_H_