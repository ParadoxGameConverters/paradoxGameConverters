/*Copyright (c) 2016 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_


#include <vector>
#include <string>
#include "Date.h"
#include "EU4World/EU4Version.h"
using namespace std;



class Configuration // Singleton
{
public:
	Configuration();

	static string getEU4Path()
	{
		return getInstance()->EU4Path;
	}

	static string getEU4DocumentsPath()
	{
		return getInstance()->EU4DocumentsPath;
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

	static string getV2Gametype()
	{
		return getInstance()->V2Gametype;
	}

	static EU4Version getEU4Version()
	{
		return getInstance()->version;
	}

	static void setEU4Version(EU4Version version)
	{
		getInstance()->version = version;
	}

	static date	getFirstEU4Date()
	{
		return getInstance()->firstEU4Date;
	}

	static void setFirstEU4Date(date _firstDate)
	{
		getInstance()->firstEU4Date = _firstDate;
	}

	static date	getLastEU4Date()
	{
		return getInstance()->lastEU4Date;
	}

	static void setLastEU4Date(date _lastDate)
	{
		getInstance()->lastEU4Date = _lastDate;
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

	static double getLibertyThreshold()
	{
		return getInstance()->libertyThreshold;
	}

	static bool getConvertPopTotals()
	{
		return getInstance()->convertPopTotals;
	}

	static void setOutputName(string name)
	{
		getInstance()->outputName = name;
	}

	static string getOutputName()
	{
		return getInstance()->outputName;
	}

	static void setActiveDLCs(vector<string> _activeDLCs)
	{
		getInstance()->activeDLCs = _activeDLCs;
	}

	static bool wasDLCActive(string DLC)
	{
		for (auto activeDLC: getInstance()->activeDLCs)
		{
			if (DLC == activeDLC)
			{
				return true;
			}
		}
		return false;
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
	string	EU4Path;					// the install directory for EU4
	string	EU4DocumentsPath;		// EU4's directory under My Documents
	string	CK2ExportPath;			// where CK2 exported game mods get put
	string	V2Path;					// the install directory for V2
	string	V2DocumentsPath;		// V2's directory under My Documents
	string	V2Gametype;				// whether V2 is vanilla, AHD, or HoD
	string	resetProvinces;		// whether or not to reset allowed provinces back to V2 defaults
	double	MaxLiteracy;			// the maximum literacy allowed
	string	Removetype;				// the ruleto use for removing excess EU4 nations
	double	libertyThreshold;		// The threshold liberty desire value for when colonies are not absorbed
	bool		convertPopTotals;		// whether or not to convert pop totals
	
	// items set during conversion
	EU4Version		version;
	date				firstEU4Date;			// the date EU4 began
	date				lastEU4Date;			// the date EU4 ended
	string			outputName;				// the name the outputted mod should have
	vector<string>	activeDLCs;				// the active DLCs in the EU4 game
};

#endif // CONFIGURATION_H_