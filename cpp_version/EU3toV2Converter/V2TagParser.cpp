#include "V2TagParser.h"

vector<string> parseV2Tags(ifstream &input)
{
	vector<string> tagList;
	while (input.good())
	{
		string line;
		getline(input, line);

		if ( (line[0] == '#') | (line.size() < 3) )
		{
			continue;
		}
		
		string tag;
		tag = line.substr(0, 3);
		if (tag == "REB")
		{
			continue;
		}
		tagList.push_back(tag);
	}
	return tagList;
}