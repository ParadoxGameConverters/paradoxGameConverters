/*Copyright (c) 2015 The Paradox Game Converters Project

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
using namespace std;



class Configuration // Singleton
{
	public:
		Configuration();

		static string getHoI3Path()
		{
			return getInstance()->HoI3Path;
		}

		static string getHoI3DocumentsPath()
		{
			return getInstance()->HoI3DocumentsPath;
		}

		static string getV2Path()
		{
			return getInstance()->V2Path;
		}

		static string getV2DocumentsPath()
		{
			return getInstance()->V2DocumentsPath;
		}

		static vector<string> getVic2Mods()
		{
			return getInstance()->Vic2Mods;
		}

		static string getResetProvinces()
		{
			return getInstance()->resetProvinces;
		}

		static void setOutputName(string name)
		{
			getInstance()->outputName = name;
		}

		static string getOutputName()
		{
			return getInstance()->outputName;
		}

		static double getMinInfra()
		{
			return getInstance()->minInfra;
		}

		static string getFactionLeaderAlgo()
		{
			return getInstance()->factionLeaderAlgorithm;
		}

		static vector<string> getManualAxisFaction()
		{
			return getInstance()->manualAxisFaction;
		}

		static vector<string> getManualAlliesFaction()
		{
			return getInstance()->manualAlliesFaction;
		}

		static vector<string> getManualCominternFaction()
		{
			return getInstance()->manualCominternFaction;
		}

		static string getIcConversion()
		{
			return getInstance()->icConversion;
		}

		static double getIcFactor()
		{
			return getInstance()->icFactor;
		}

		static string getManpowerConversion()
		{
			return getInstance()->manpowerConversion;
		}

		static double getManpowerFactor()
		{
			return getInstance()->manpowerFactor;
		}

		static string getLeadershipConversion()
		{
			return getInstance()->leadershipConversion;
		}

		static double getLeadershipFactor()
		{
			return getInstance()->leadershipFactor;
		}

		static double getLiteracyWeight()
		{
			return getInstance()->literacyWeight;
		}

		static double getPracticalsScale()
		{
			return getInstance()->practicalsScale;
		}

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

		string			HoI3Path;				// the install directory for HOI3
		string			HoI3DocumentsPath;	// HoI3's directory under My Documents
		string			V2Path;					// the install directory for V2
		string			V2DocumentsPath;		// V2's directory under My Documents
		vector<string>	Vic2Mods;
		string			resetProvinces;		// whether or not to reset allowed provinces back to V2 defaults
		string			outputName;				// the name the outputted mod should have

		string			factionLeaderAlgorithm;
		vector<string> manualAxisFaction;
		vector<string> manualAlliesFaction;
		vector<string> manualCominternFaction;
		double			minInfra;
		string			icConversion;
		double			icFactor;
		string			manpowerConversion;
		double			manpowerFactor;
		string			leadershipConversion;
		double			leadershipFactor;
		double			literacyWeight;
		double			practicalsScale;
};

#endif // CONFIGURATION_H_