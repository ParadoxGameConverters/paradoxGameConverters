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
	string	EU4DocumentsPath;
	string	CK2ExportPath;
	string	V2Path;
	string	V2DocumentsPath;
	string	V2Gametype;
	bool		CK2Converted;
	string	resetProvinces;
	double	MaxLiteracy;
	string	Removetype;
	
	// items set during conversion
	date		firstEU4Date;
	string	outputName;
};

#endif // CONFIGURATION_H_