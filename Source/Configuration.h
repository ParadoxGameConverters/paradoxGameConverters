#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_


#include <string>
#include "Date.h"
using namespace std;



class Configuration // Singleton
{
public:
	Configuration();

	static string getEU4Path()
	{
		return getInstance()->EU4Path;
	}

	static string getV2Path()
	{
		return getInstance()->V2Path;
	}

	static string getEU4Gametype()
	{
		return getInstance()->EU4Gametype;
	}

	static string getV2Gametype()
	{
		return getInstance()->V2Gametype;
	}

	static date getStartDate()
	{
		return getInstance()->StartDate;
	}

	static double getMaxLiteracy()
	{
		return getInstance()->MaxLiteracy;
	}

	static string getRemovetype()
	{
		return getInstance()->Removetype;
	}

	static Configuration* getInstance()
	{
		if (instance == NULL)
			instance = new Configuration();
		return instance;
	}

private:
	static Configuration* instance;

	string	EU4Path;
	string	V2Path;
	string	EU4Gametype;
	string	V2Gametype;
	string	Removetype;
	date		StartDate;
	double	MaxLiteracy;

};

#endif // CONFIGURATION_H_