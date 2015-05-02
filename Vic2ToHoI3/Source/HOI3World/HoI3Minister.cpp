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


#include "HoI3Minister.h"
#include "..\Configuration.h"



HoI3Minister::HoI3Minister(vector<string>& firstNames, vector<string>& lastNames, string _ideology, governmentJob job, governmentJobsMap& jobMap, vector<string>& portraits)
{
	ID			= Configuration::getNextLeaderID();
	name		= firstNames[rand() % firstNames.size()] + " " + lastNames[rand() % lastNames.size()];
	ideology	= _ideology;
	loyalty	= 1.0f;

	picture	= portraits[rand() % portraits.size()];

	vector<string> traits = jobMap.find(job.first)->second;
	roles.push_back(make_pair(job.first, traits[rand() % traits.size()]));

	governmentJobsMap::iterator secondJob = jobMap.begin();
	std::advance(secondJob, rand() % jobMap.size());
	while (job.first == secondJob->first)
	{
		secondJob = jobMap.begin();
		std::advance(secondJob, rand() % jobMap.size());
	}
	traits = secondJob->second;
	roles.push_back(make_pair(secondJob->first, traits[rand() % traits.size()]));
}


void HoI3Minister::output(FILE * output)
{
	fprintf(output, "\t%d = {\n", ID);
	fprintf(output, "\t\tname = \"%s\"\n", name.c_str());
	fprintf(output, "\t\tideology = %s\n", ideology.c_str());
	fprintf(output, "\t\tloyalty = %f\n", loyalty);
	fprintf(output, "\t\tpicture = %s\n", picture.c_str());
	for (auto role: roles)
	{
		fprintf(output, "\t\t%s = %s\n", role.first.c_str(), role.second.c_str());
	}
	fprintf(output, "\t\tstart_date = 1936.1.1\n");
	fprintf(output, "\t}\n");
}