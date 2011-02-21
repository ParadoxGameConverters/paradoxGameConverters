#include "V2Province.h"


void V2Province::init(Object* obj) {
	num = atoi(obj->getKey().c_str());

	vector<Object*> nameObj;
	nameObj = obj->getValue("name");
	if (nameObj.size() == 0)
	{
		name = "";
	}
	else
	{
		name = nameObj[0]->getLeaf();
	}

	vector<Object*> rgoObj;
	rgoObj = obj->getValue("rgo");
	if (rgoObj.size() == 0)
	{
		land = false;
	}
	else
	{
		land = true;
		vector<Object*> goodsObj= rgoObj[0]->getValue("goods_type");
		rgoType = goodsObj[0]->getLeaf();
		lifeRating = atoi( obj->getValue("life_rating")[0]->getLeaf().c_str() );
	}

	oldPopulation = 0;
	vector<Object*> leaves = obj->getLeaves();
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( ( key == "aristocrats") || ( key == "artisans") || ( key == "bureaucrats") || ( key == "") || ( key == "clergymen") || ( key == "clerks") || ( key == "craftsmen") || ( key == "farmers") || ( key == "labourers") || ( key == "officers") || ( key == "slaves") || ( key == "soldiers") )
		{
			oldPopulation += atoi( (leaves[i]->getValue("size"))[0]->getLeaf().c_str() );
		}
	}
}


int V2Province::getNum()
{
	return num;
}


void V2Province::setOwner(string newOwner)
{
	owner = newOwner;
}


string V2Province::getOwner()
{
	return owner;
}


void V2Province::output(FILE* output)
{
	fprintf(output, "%d=\n", num);
	fprintf(output, "{\n");
	fprintf(output, "	name=%s\n", name.c_str());
	if (land)
	{
		if (owner != "")
		{
			fprintf(output, "	owner=\"%s\"\n", owner.c_str());
			fprintf(output, "	controller=\"%s\"\n", owner.c_str());
			fprintf(output, "	core=\"%s\"\n", owner.c_str());
		}
	}
	fprintf(output, "	garrison=100.000\n");
	if (land)
	{
		outputPopStuff(output, oldPopulation, rgoType);
		fprintf(output, "	rgo=\n");
		fprintf(output, "	{\n");
		fprintf(output, "		goods_type=%s\n", rgoType.c_str());
		fprintf(output, "	}\n");
		fprintf(output, "	life_rating=%d\n", lifeRating);
	}
	fprintf(output, "}\n");
}