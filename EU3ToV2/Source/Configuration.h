/*Copyright (c) 2014 The Paradox Game Converters Project

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

#include <string>
#include "Date.h"

using namespace std;

class Configuration // Singleton
{
	public:
		Configuration();

		static date		getFirstEU3Date()							{ return getInstance()->firstEU3Date; }
		static void		setFirstEU3Date(date _firstDate)		{ getInstance()->firstEU3Date = _firstDate; }
		static string	getResetProvinces()						{ return getInstance()->resetProvinces; }
		static double	getMaxLiteracy()							{ return getInstance()->MaxLiteracy; }
		static string	getV2Path()									{ return getInstance()->V2Path; }
		static string	getEU3Path()								{ return getInstance()->EU3Path; }
		static string	getEU3Mod()									{ return getInstance()->EU3Mod; }
		static string	getV2DocumentsPath()						{ return getInstance()->V2DocumentsPath; }
		static string	getEU3Gametype()							{ return getInstance()->EU3gametype; }
		static string	getV2Gametype()							{ return getInstance()->V2Gametype; }
		static string	getRemovetype()							{ return getInstance()->Removetype; }
		static string	getOutputName()							{ return getInstance()->outputName; }
		static bool getConvertPopTotals()						{ return getInstance()->convertPopTotals; }
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

		// options from configuration.txt
		string	EU3Path;					// the install directory for EU3
		string	EU3Mod;					// the name of the EU3 mod to use for conversion
		string	V2Path;					// the install directory for V2
		string	V2DocumentsPath;		// V2's directory under My Documents
		string	V2Gametype;				// whether V2 is vanilla, AHD, or HoD
		string	resetProvinces;		// whether or not to reset allowed provinces back to V2 defaults
		double	MaxLiteracy;			// the maximum literacy allowed
		string	Removetype;				// the rule to use for removing excess EU3 nations
		bool		convertPopTotals;		// whether or not to convert pop totals

		// items set during conversion
		date		firstEU3Date;
		string	EU3gametype;
		string	outputName;
};

#endif // CONFIGURATION_H_