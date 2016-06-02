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


#include "HoI4Minister.h"
#include "..\Configuration.h"



HoI4Minister::HoI4Minister(vector<wstring>& firstNames, vector<wstring>& lastNames, wstring _ideology, governmentJob job, governmentJobsMap& jobMap, vector<wstring>& portraits)
{
	ID			= Configuration::getNextLeaderID();
	name		= firstNames[rand() % firstNames.size()] + L" " + lastNames[rand() % lastNames.size()];
	ideology	= _ideology;
	loyalty	= 1.0f;

	picture	= portraits[rand() % portraits.size()];

	vector<wstring> traits = jobMap.find(job.first)->second;
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


void HoI4Minister::output(FILE * output)
{
	fwprintf(output, L"\t%d = {\n", ID);
	fwprintf(output, L"\t\tname = \"%s\"\n", name.c_str());
	fwprintf(output, L"\t\tideology = %s\n", ideology.c_str());
	fwprintf(output, L"\t\tloyalty = %.2f\n", loyalty);
	fwprintf(output, L"\t\tpicture = %s\n", picture.c_str());
	for (auto role: roles)
	{
		fwprintf(output, L"\t\t%s = %s\n", role.first.c_str(), role.second.c_str());
	}
	fwprintf(output, L"\t\tstart_date = 1936.1.1\n");
	fwprintf(output, L"\t}\n");
}