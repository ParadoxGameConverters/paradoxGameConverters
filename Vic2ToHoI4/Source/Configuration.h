/*Copyright (c) 2018 The Paradox Game Converters Project

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



#include "newParser.h"
#include "HOI4World/HOI4Version.h"
#include <string>
#include <vector>



enum class ideologyOptions
{
	keep_major,
	keep_all,
	specified,
	keep_default
};



class Configuration: commonItems::parser
{
	public:
		Configuration() = default;
		void instantiate(std::istream& theStream);

		std::string getHoI4Path() const { return HoI4Path; }
		std::string getHoI4DocumentsPath() const { return HoI4DocumentsPath; }
		std::string getVic2Path() const { return Vic2Path; }
		std::vector<std::string> getVic2Mods() const { return Vic2Mods; }
		std::string getOutputName() const { return outputName; }

		double getForceMultiplier() const { return forceMultiplier; }
		double getManpowerFactor() const { return manpowerFactor; }
		double getIndustrialShapeFactor() const { return industrialShapeFactor; }
		double getIcFactor() const { return icFactor; }
		ideologyOptions getIdeologiesOptions() const { return ideologiesOptions; }
		std::vector<std::string> getSpecifiedIdeologies() const { return specifiedIdeologies; }
		bool getRemoveCores() const { return removeCores; }
		bool getCreateFactions() const { return createFactions; }
		HoI4::Version getHOI4Version() const { return version; }

		bool getDebug() const { return debug; }

		int getNextLeaderID() { return leaderID++; }

		void setForceMultiplier(double mult) { forceMultiplier = mult; }
		void setOutputName(const std::string& name) { outputName = name; }
		void setLeaderIDForNextCountry() {
			leaderIDCountryIdx++;
			leaderID = 1000 * leaderIDCountryIdx;	
		}

	private:
		Configuration(const Configuration&) = delete;
		Configuration& operator=(const Configuration&) = delete;

		HoI4::Version getAutomaticHoI4Version();

		std::string HoI4Path = "";
		std::string HoI4DocumentsPath = "";
		std::string Vic2Path = "";
		std::vector<std::string> Vic2Mods;
		std::string outputName = "";

		double forceMultiplier = 0.0;
		double manpowerFactor = 0.0;
		double industrialShapeFactor = 0.0;
		double icFactor = 0.0;
		ideologyOptions ideologiesOptions = ideologyOptions::keep_major;
		std::vector<std::string> specifiedIdeologies;
		bool removeCores = true;
		bool createFactions = true;
		HoI4::Version version;

		bool debug = false;

		unsigned int leaderID = 1000;
		unsigned int leaderIDCountryIdx = 1;
};


extern Configuration theConfiguration;


class ConfigurationFile: commonItems::parser
{
	public:
		explicit ConfigurationFile(const std::string& filename);

	private:
		ConfigurationFile(const ConfigurationFile&) = delete;
		ConfigurationFile& operator=(const ConfigurationFile&) = delete;
};



#endif // CONFIGURATION_H_