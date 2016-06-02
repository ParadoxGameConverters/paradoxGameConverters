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


#include <string>
#include <vector>
using namespace std;



class Configuration // Singleton
{
	public:
		Configuration();

		static wstring getHoI4Path()
		{
			return getInstance()->HoI4Path;
		}

		static wstring getHoI4DocumentsPath()
		{
			return getInstance()->HoI4DocumentsPath;
		}

		static wstring getV2Path()
		{
			return getInstance()->V2Path;
		}

		static wstring getV2DocumentsPath()
		{
			return getInstance()->V2DocumentsPath;
		}

		static vector<wstring> getVic2Mods()
		{
			return getInstance()->Vic2Mods;
		}

		static void setOutputName(wstring name)
		{
			getInstance()->outputName = name;
		}

		static wstring getOutputName()
		{
			return getInstance()->outputName;
		}

		static double getMinInfra()
		{
			return getInstance()->minInfra;
		}

		static wstring getFactionLeaderAlgo()
		{
			return getInstance()->factionLeaderAlgorithm;
		}

		static vector<wstring> getManualAxisFaction()
		{
			return getInstance()->manualAxisFaction;
		}

		static vector<wstring> getManualAlliesFaction()
		{
			return getInstance()->manualAlliesFaction;
		}

		static vector<wstring> getManualCominternFaction()
		{
			return getInstance()->manualCominternFaction;
		}

		static wstring getIcConversion()
		{
			return getInstance()->icConversion;
		}

		static double getIcFactor()
		{
			return getInstance()->icFactor;
		}

		static wstring getManpowerConversion()
		{
			return getInstance()->manpowerConversion;
		}

		static double getManpowerFactor()
		{
			return getInstance()->manpowerFactor;
		}

		static wstring getLeadershipConversion()
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

		static int getNextLeaderID()
		{
			return getInstance()->leaderID++;
		}

		static void setLeaderIDForNextCountry()
		{
			getInstance()->leaderIDCountryIdx++;
			getInstance()->leaderID = 1000 * getInstance()->leaderIDCountryIdx;	
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

		wstring			HoI4Path;				// the install directory for HoI4
		wstring			HoI4DocumentsPath;	// HoI4's directory under My Documents
		wstring			V2Path;					// the install directory for V2
		wstring			V2DocumentsPath;		// V2's directory under My Documents
		vector<wstring>	Vic2Mods;
		wstring			outputName;				// the name the outputted mod should have

		wstring			factionLeaderAlgorithm;
		vector<wstring> manualAxisFaction;
		vector<wstring> manualAlliesFaction;
		vector<wstring> manualCominternFaction;
		double			minInfra;
		wstring			icConversion;
		double			icFactor;
		wstring			manpowerConversion;
		double			manpowerFactor;
		wstring			leadershipConversion;
		double			leadershipFactor;
		double			literacyWeight;
		double			practicalsScale;

		unsigned int	leaderID;
		unsigned int	leaderIDCountryIdx;
};

#endif // CONFIGURATION_H_