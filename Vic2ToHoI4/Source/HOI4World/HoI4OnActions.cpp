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



#include "HoI4OnActions.h"
#include "Log.h"
#include "../Configuration.h"
#include <fstream>
using namespace std;



HoI4OnActions::HoI4OnActions(): electionEvents{ "election.2", "election.3" }
{
}


void HoI4OnActions::output() const
{
	ofstream onActionsFile("output/" + Configuration::getOutputName() + "/common/on_actions/99_converter_on_actions.txt");
	if (!onActionsFile.is_open())
	{
		LOG(LogLevel::Error) << "Could not create NF_events.txt";
		exit(-1);
	}

	onActionsFile << "on_actions = {\n";
	onActionsFile << "	# country\n";
	onActionsFile << "	on_new_term_election = {\n";
	onActionsFile << "		random_events = {\n";
	for (auto event: electionEvents)
	{
		onActionsFile << "			100 = " << event << "\n";
	}
	onActionsFile << "		}\n";
	onActionsFile << "	}\n";
	onActionsFile << "}\n";

	onActionsFile.close();
}