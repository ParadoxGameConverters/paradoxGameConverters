/*Copyright (c) 2013 The CK2 to EU3 Converter Project
 
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



#include "EU3History.h"
#include "..\CK2World\CK2Character.h"
#include "..\CK2World\CK2History.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"



EU3History::EU3History(date _when)
{
	when				= _when;
	monarch			= NULL;
	regent			= NULL;
	heir				= NULL;
	advisor			= NULL;
	capital			= "";
	tradeGood		= "";
	baseTax			= 0.0f;
	population		= 0.0f;
	manpower			= 0;
	owner				= "";
	add_core			= "";
	remove_core		= "";
	government		= "";
	culture			= "";
	religion			= "";
	primaryCulture	= "";
	techGroup		= "";
	acceptedCultures.clear();
	discoverers.clear();
	shogunPower		= -1.0f;
}


EU3History::EU3History(CK2History* src)
{
	when = src->getWhen();

	monarch	= NULL;
	regent	= NULL;
	heir		= NULL;
	advisor	= NULL;

	CK2Character* holder = src->getHolder();
	if (holder != NULL)
	{
		CK2Character* CK2Regent = holder->getRegent();
		if (CK2Regent != NULL)
		{
			regent	= new EU3Ruler(CK2Regent);
			heir		= new EU3Ruler(holder);
		}
		else
		{
			monarch = new EU3Ruler(holder);
		}
	}
	capital			= "";
	tradeGood		= "";
	baseTax			= 0.0f;
	population		= 0.0;
	manpower			= 0;
	owner				= "";
	government		= "";
	culture			= "";
	religion			= "";
	primaryCulture	= "";
	techGroup		= "";
	acceptedCultures.clear();
	discoverers.clear();
	shogunPower		= -1.0f;
}


void EU3History::output(FILE* output)
{
	fprintf(output, "\t\t%d.%d.%d=\n", when.year, when.month, when.day);
	fprintf(output, "\t\t{\n");
	if (monarch != NULL)
	{
		monarch->outputAsMonarch(output);
	}
	if (regent != NULL)
	{
		regent->outputAsRegent(output);
		fprintf(output, "\t\t}\n");
		fprintf(output, "\t\t%d.%d.%d=\n", when.year, when.month, when.day);
		fprintf(output, "\t\t{\n");
	}
	if (heir != NULL)
	{
		heir->outputAsHeir(output);
	}
	if (advisor != NULL)
	{
		advisor->outputInProvince(output);
	}
	if (capital != "")
	{
		fprintf(output, "\t\t\tcapital=\"%s\"\n", capital.c_str());
	}
	if (tradeGood != "")
	{
		fprintf(output, "\t\t\ttrade_goods = %s\n", tradeGood.c_str());
	}
	if (population != 0.0)
	{
		fprintf(output, "\t\t\tcitysize=\"%f\"\n", population);
	}
	if (baseTax != 0.0)
	{
		fprintf(output, "\t\t\tbase_tax=%f\n", baseTax);
	}
	if (manpower != 0)
	{
		fprintf(output, "\t\t\tmanpower=%d\n", manpower);
	}
	if (owner != "")
	{
		fprintf(output, "\t\t\towner=\"%s\"\n", owner.c_str());
	}
	if (add_core != "")
	{
		fprintf(output, "\t\t\tadd_core=\"%s\"\n", add_core.c_str());
	}
	if (remove_core != "")
	{
		fprintf(output, "\t\t\tremove_core=\"%s\"\n", remove_core.c_str());
	}
	if (government != "")
	{
		fprintf(output, "\t\t\tgovernment=\"%s\"\n", government.c_str());
	}
	if (culture != "")
	{
		fprintf(output, "\t\t\tculture=%s\n", culture.c_str());
	}
	for (unsigned int i = 0; i < acceptedCultures.size(); i++)
	{
		fprintf(output, "\t\t\tadd_accepted_culture=%s\n", acceptedCultures[i].c_str());
	}
	if (religion != "")
	{
		fprintf(output, "\t\t\treligion=%s\n", religion.c_str());
	}
	if (primaryCulture != "")
	{
		fprintf(output, "\t\t\tprimary_culture=%s\n", primaryCulture.c_str());
	}
	if (techGroup != "")
	{
		fprintf(output, "\t\t\ttechnology_group=%s\n", techGroup.c_str());
	}
	for (unsigned int i = 0; i < discoverers.size(); i++)
	{
		fprintf(output, "\t\t\tdiscovered_by=\"%s\"\n", discoverers[i].c_str());
	}
	if (shogunPower != -1.0f)
	{
		fprintf(output, "\t\t\tshogun=%f\n", shogunPower);
	}
	fprintf(output, "\t\t}\n");
}