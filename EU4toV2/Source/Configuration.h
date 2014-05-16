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

	static string getEU4ModPath()
	{
		return getInstance()->EU4ModPath;
	}

	static string getCK2ExportPath()
	{
		return getInstance()->CK2ExportPath;
	}

	static string getV2Path()
	{
		return getInstance()->V2Path;
	}

	static string getV2DocumentsPath()
	{
		return getInstance()->V2DocumentsPath;
	}

	static bool getCK2Converted()
	{
		return getInstance()->CK2Converted;
	}

	static date	getFirstEU4Date()
	{
		return getInstance()->firstEU4Date;
	}

	static void setFirstEU4Date(date _firstDate)
	{
		getInstance()->firstEU4Date = _firstDate;
	}

	static string getResetProvinces()
	{
		return getInstance()->resetProvinces;
	}

	static double getMaxLiteracy()
	{
		return getInstance()->MaxLiteracy;
	}

	static string getRemovetype()
	{
		return getInstance()->Removetype;
	}

	static void setOutputName(string name)
	{
		getInstance()->outputName = name;
	}

	static string getOutputName()
	{
		return getInstance()->outputName;
	}

	static Configuration* getInstance()
	{
		if (instance == NULL)
			instance = new Configuration();
		return instance;
	}

private:
	static Configuration* instance;

	// options from configuration.txt
	string	EU4Path;
	string	EU4ModPath;
	string	CK2ExportPath;
	string	V2Path;
	string	V2DocumentsPath;
	bool		CK2Converted;
	string	resetProvinces;
	double	MaxLiteracy;
	string	Removetype;
	
	// items set during conversion
	date		firstEU4Date;
	string	outputName;
};

#endif // CONFIGURATION_H_