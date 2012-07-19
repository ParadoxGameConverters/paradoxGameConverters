#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include "Date.h"

using namespace std;

class Configuration // Singleton
{
	public:
		Configuration();

		static date getStartDate()			const { return getInstance()->startDate; }
		static double getMaxLiteracy()	const { return getInstance()->maxLiteracy; }
		static string getV2Path()			const { return getInstance()->V2Path; }
		static string getEU3Path()			const { return getInstance()->EU3Path;	}
		static string getEU3Gametype()	const { return getInstance()->EU3gametype; }
		static string getV2Gametype()		const { return getInstance()->V2gametype;	}
		static string getRemovetype()		const { return getInstance()->removetype;	}
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
		double	maxLiteracy;
		string	V2Path;
		string	EU3Path;
		string	EU3gametype;
		string	V2gametype;
		string	removetype;
};

#endif // CONFIGURATION_H_