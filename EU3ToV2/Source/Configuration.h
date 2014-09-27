#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include "Date.h"

using namespace std;

class Configuration // Singleton
{
	public:
		Configuration();

		static date		getStartDate()								{ return getInstance()->startDate; }
		static date		getFirstEU3Date()							{ return getInstance()->firstEU3Date; }
		static void		setFirstEU3Date(date _firstDate)		{ getInstance()->firstEU3Date = _firstDate; }
		static string	getResetProvinces()						{ return getInstance()->resetProvinces; }
		static double	getMaxLiteracy()							{ return getInstance()->maxLiteracy; }
		static string	getV2Path()									{ return getInstance()->V2Path; }
		static string	getEU3Path()								{ return getInstance()->EU3Path; }
		static string	getEU3Mod()									{ return getInstance()->EU3Mod; }
		static bool		getUseV2Mod()								{ return getInstance()->useV2Mod; }
		static string	getV2DocumentsPath()						{ return getInstance()->V2DocumentsPath; }
		static string	getEU3Gametype()							{ return getInstance()->EU3gametype; }
		static string	getV2Gametype()							{ return getInstance()->V2gametype; }
		static string	getRemovetype()							{ return getInstance()->removetype; }
		static string	getOutputName()							{ return getInstance()->outputName; }
		static void		setOutputName(string _outputName)	{ getInstance()->outputName = _outputName; }

		static Configuration* getInstance()
		{
			if (instance == NULL)
			{
				instance = new Configuration();
			}
			return instance;
		}

	private:
		static Configuration* instance;

		date		startDate;
		date		firstEU3Date;
		string	resetProvinces;
		double	maxLiteracy;
		string	V2Path;
		string	V2DocumentsPath;
		string	EU3Path;
		string	EU3gametype;
		string	V2gametype;
		string	EU3Mod;
		bool		useV2Mod;
		string	removetype;
		string	outputName;
};

#endif // CONFIGURATION_H_