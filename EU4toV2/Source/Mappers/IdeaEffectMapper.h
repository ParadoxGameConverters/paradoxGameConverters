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



#ifndef IDEA_EFFECT_MAPPER_H
#define IDEA_EFFECT_MAPPER_H



#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;



class Object;



class ideaEffectMapper
{
	public:
		static int getArmyInvestmentFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetArmyInvestmentFromIdea(ideaName, ideaLevel);
		}

		static int getCommerceInvestmentFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetCommerceInvestmentFromIdea(ideaName, ideaLevel);
		}

		static int getCultureInvestmentFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetCultureInvestmentFromIdea(ideaName, ideaLevel);
		}

		static int getIndustryInvestmentFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetIndustryInvestmentFromIdea(ideaName, ideaLevel);
		}

		static int getNavyInvestmentFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetNavyInvestmentFromIdea(ideaName, ideaLevel);
		}

		static double getUHLiberalFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetUHLiberalFromIdea(ideaName, ideaLevel);
		}

		static double getUHReactionaryFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetUHReactionaryFromIdea(ideaName, ideaLevel);
		}

		static double getLiteracyFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetLiteracyFromIdea(ideaName, ideaLevel);
		}

		static int getOrderInfluenceFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetOrderInfluenceFromIdea(ideaName, ideaLevel);
		}

		static int getLibertyInfluenceFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetLibertyInfluenceFromIdea(ideaName, ideaLevel);
		}

		static int getEqualityInfluenceFromIdea(const string& ideaName, int ideaLevel)
		{
			return getInstance()->GetEqualityInfluenceFromIdea(ideaName, ideaLevel);
		}

		static double getArmyTechFromIdeas(const map<string, int>& ideas)
		{
			return getInstance()->GetArmyTechFromIdeas(ideas);
		}

		static double getCommerceTechFromIdeas(const map<string, int>& ideas)
		{
			return getInstance()->GetCommerceTechFromIdeas(ideas);
		}

		static double getCultureTechFromIdeas(const map<string, int>& ideas)
		{
			return getInstance()->GetCultureTechFromIdeas(ideas);
		}

		static double getIndustryTechFromIdeas(const map<string, int>& ideas)
		{
			return getInstance()->GetIndustryTechFromIdeas(ideas);
		}

		static double getNavyTechFromIdeas(const map<string, int>& ideas)
		{
			return getInstance()->GetNavyTechFromIdeas(ideas);
		}

	private:
		static ideaEffectMapper* instance;
		static ideaEffectMapper* getInstance()
		{
			if (instance == nullptr)
			{
				instance = new ideaEffectMapper;
			}
			return instance;
		}

		ideaEffectMapper();
		void initIdeaEffects(shared_ptr<Object> obj);

		int GetArmyInvestmentFromIdea(const string& ideaName, int ideaLevel);
		int GetCommerceInvestmentFromIdea(const string& ideaName, int ideaLevel);
		int GetCultureInvestmentFromIdea(const string& ideaName, int ideaLevel);
		int GetIndustryInvestmentFromIdea(const string& ideaName, int ideaLevel);
		int GetNavyInvestmentFromIdea(const string& ideaName, int ideaLevel);

		double GetUHLiberalFromIdea(const string& ideaName, int ideaLevel);
		double GetUHReactionaryFromIdea(const string& ideaName, int ideaLevel);

		double GetLiteracyFromIdea(const string& ideaName, int ideaLevel);

		int GetOrderInfluenceFromIdea(const string& ideaName, int ideaLevel);
		int GetLibertyInfluenceFromIdea(const string& ideaName, int ideaLevel);
		int GetEqualityInfluenceFromIdea(const string& ideaName, int ideaLevel);

		double GetArmyTechFromIdeas(const map<string, int>& ideas);
		double GetCommerceTechFromIdeas(const map<string, int>& ideas);
		double GetCultureTechFromIdeas(const map<string, int>& ideas);
		double GetIndustryTechFromIdeas(const map<string, int>& ideas);
		double GetNavyTechFromIdeas(const map<string, int>& ideas);


		map<string, int> armyInvestmentIdeas;
		map<string, int> commerceInvestmentIdeas;
		map<string, int> cultureInvestmentIdeas;
		map<string, int> industryInvestmentIdeas;
		map<string, int> navyInvestmentIdeas;

		map<string, double> UHLiberalIdeas;
		map<string, double> UHReactionaryIdeas;

		map<string, vector<int>> literacyIdeas;

		map<string, int> orderIdeas;
		map<string, int> libertyIdeas;
		map<string, int> equalityIdeas;

		map<string, double> armyTechIdeas;
		map<string, double> commerceTechIdeas;
		map<string, double> cultureTechIdeas;
		map<string, double> industryTechIdeas;
		map<string, double> navyTechIdeas;
};



#endif // IDEA_EFFECT_MAPPER_H
