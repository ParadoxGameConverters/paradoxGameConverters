/*Copyright (c) 2017 The Paradox Game Converters Project

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
#include <vector>
#include "HOI4World/HOI4Version.h"
using namespace std;



enum class ideologyOptions
{
	keep_major,
	keep_all,
	keep_default
};



class Configuration // Singleton
{
	public:
		Configuration();

		static string getHoI4Path()
		{
			return getInstance()->HoI4Path;
		}

		static string getHoI4DocumentsPath()
		{
			return getInstance()->HoI4DocumentsPath;
		}

		static string getV2Path()
		{
			return getInstance()->V2Path;
		}

		static vector<string> getVic2Mods()
		{
			return getInstance()->Vic2Mods;
		}

		static void setForceMultiplier(double mult)
		{
			getInstance()->forceMultiplier = mult;
		}

		static void setOutputName(const string& name)
		{
			getInstance()->outputName = name;
		}

  		static double getForceMultiplier()
		{
			return getInstance()->forceMultiplier;
		}

		static string getOutputName()
		{
			return getInstance()->outputName;
		}

		static double getManpowerFactor()
		{
			return getInstance()->manpowerFactor;
		}

		static double getIndustrialShapeFactor()
		{
			return getInstance()->industrialShapeFactor;
		}

		static double getIcFactor()
		{
			return getInstance()->icFactor;
		}

		static int getNextLeaderID()
		{
			return getInstance()->leaderID++;
		}

		static void setLeaderIDForNextCountry()
		{
			getInstance()->leaderIDCountryIdx++;
			getInstance()->leaderID = 1000 * getInstance()->leaderIDCountryIdx;	
		}

		static HOI4Version& getHOI4Version()
		{
			return getInstance()->version;
		}

		static ideologyOptions getIdeologiesOptions()
		{
			return getInstance()->ideologiesOptions;
		}

		static bool getDebug()
		{
			return getInstance()->debug;
		}

		static bool getRemoveCores()
		{
			return getInstance()->removeCores;
		}

		static Configuration* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new Configuration();
			}
			return instance;
		}

	private:
		static Configuration* instance;
		Configuration(const Configuration&) = delete;
		Configuration& operator=(const Configuration&) = delete;

		HOI4Version getAutomaticHoI4Version();

		string			HoI4Path;				// the install directory for HoI4
		string			HoI4DocumentsPath;	// HoI4's directory under My Documents
		string			V2Path;					// the install directory for V2
		vector<string>	Vic2Mods;
		string			outputName;				// the name the outputted mod should have

		double			forceMultiplier;
		double			manpowerFactor;
		double industrialShapeFactor;
		double			icFactor;

		ideologyOptions ideologiesOptions;

		bool debug;
		bool removeCores;

		unsigned int	leaderID;
		unsigned int	leaderIDCountryIdx;

		HOI4Version version;
};



#endif // CONFIGURATION_H_