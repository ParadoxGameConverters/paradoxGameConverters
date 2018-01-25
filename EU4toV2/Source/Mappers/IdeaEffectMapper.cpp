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



#include "IdeaEffectMapper.h"
#include "Log.h"
#include "Object.h"
#include "ParadoxParserUTF8.h"



ideaEffectMapper* ideaEffectMapper::instance = nullptr;


ideaEffectMapper::ideaEffectMapper()
{
	LOG(LogLevel::Info) << "Getting idea effects";
	shared_ptr<Object> ideaObj = parser_UTF8::doParseFile("idea_effects.txt");
	if (ideaObj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file idea_effects.txt";
		exit(-1);
	}
	
	initIdeaEffects(ideaObj);
}


void ideaEffectMapper::initIdeaEffects(shared_ptr<Object> obj)
{
	vector<shared_ptr<Object>> ideasObj = obj->getLeaves();
	for (auto ideasItr: obj->getLeaves())
	{
		string idea = ideasItr->getKey();

		for (auto effectsItr: ideasItr->getLeaves())
		{
			string effectType = effectsItr->getKey();

			if (effectType == "army_investment")
			{
				armyInvestmentIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "commerce_investment")
			{
				commerceInvestmentIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "culture_investment")
			{
				cultureInvestmentIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "industry_investment")
			{
				industryInvestmentIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "navy_investment")
			{
				navyInvestmentIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "army_tech_score")
			{
				armyTechIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "commerce_tech_score")
			{
				commerceTechIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "culture_tech_score")
			{
				cultureTechIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "industry_tech_score")
			{
				industryTechIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "navy_tech_score")
			{
				navyTechIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "upper_house_liberal")
			{
				UHLiberalIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "upper_house_reactionary")
			{
				UHReactionaryIdeas[idea] = stof(effectsItr->getLeaf());
			}
			else if (effectType == "NV_order")
			{
				orderIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "NV_liberty")
			{
				libertyIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "NV_equality")
			{
				equalityIdeas[idea] = stoi(effectsItr->getLeaf());
			}
			else if (effectType == "literacy")
			{
				vector<string> literacyStrs = effectsItr->getTokens();
				for (auto literacyStr: literacyStrs)
				{
					literacyIdeas[idea].push_back(stoi(literacyStr));
				}
			}
		}
	}
}


int ideaEffectMapper::GetArmyInvestmentFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = armyInvestmentIdeas.find(ideaName);
	if (idea != armyInvestmentIdeas.end())
	{
		return idea->second * ideaLevel;
	}
	else
	{
		return 0;
	}
}


int ideaEffectMapper::GetCommerceInvestmentFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = commerceInvestmentIdeas.find(ideaName);
	if (idea != commerceInvestmentIdeas.end())
	{
		return idea->second * ideaLevel;
	}
	else
	{
		return 0;
	}
}


int ideaEffectMapper::GetCultureInvestmentFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = cultureInvestmentIdeas.find(ideaName);
	if (idea != cultureInvestmentIdeas.end())
	{
		return idea->second * ideaLevel;
	}
	else
	{
		return 0;
	}
}


int ideaEffectMapper::GetIndustryInvestmentFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = industryInvestmentIdeas.find(ideaName);
	if (idea != industryInvestmentIdeas.end())
	{
		return idea->second * ideaLevel;
	}
	else
	{
		return 0;
	}
}


int ideaEffectMapper::GetNavyInvestmentFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = navyInvestmentIdeas.find(ideaName);
	if (idea != navyInvestmentIdeas.end())
	{
		return idea->second * ideaLevel;
	}
	else
	{
		return 0;
	}
}


double ideaEffectMapper::GetUHLiberalFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = UHLiberalIdeas.find(ideaName);
	if (idea != UHLiberalIdeas.end())
	{
		return idea->second * (ideaLevel + 1);
	}
	else
	{
		return 0;
	}
}


double ideaEffectMapper::GetUHReactionaryFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = UHReactionaryIdeas.find(ideaName);
	if (idea != UHReactionaryIdeas.end())
	{
		return idea->second * (ideaLevel + 1);
	}
	else
	{
		return 0;
	}
}


double ideaEffectMapper::GetLiteracyFromIdea(const string& ideaName, int ideaLevel)
{
	double literacy = 0.0;

	auto idea = literacyIdeas.find(ideaName);
	if (idea != literacyIdeas.end())
	{
		for (auto level: idea->second)
		{
			if (ideaLevel >= level)
			{
				literacy += 0.1;
			}
		}
	}

	return literacy;
}


int ideaEffectMapper::GetOrderInfluenceFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = orderIdeas.find(ideaName);
	if (idea != orderIdeas.end())
	{
		if (ideaLevel == 7)
		{
			ideaLevel++;
		}
		return (ideaLevel + 1) * idea->second;
	}
	else
	{
		return 0;
	}
}


int ideaEffectMapper::GetLibertyInfluenceFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = libertyIdeas.find(ideaName);
	if (idea != libertyIdeas.end())
	{
		if (ideaLevel == 7)
		{
			ideaLevel++;
		}
		return (ideaLevel + 1) * idea->second;
	}
	else
	{
		return 0;
	}
}


int ideaEffectMapper::GetEqualityInfluenceFromIdea(const string& ideaName, int ideaLevel)
{
	auto idea = equalityIdeas.find(ideaName);
	if (idea != equalityIdeas.end())
	{
		if (ideaLevel == 7)
		{
			ideaLevel++;
		}
		return (ideaLevel + 1) * idea->second;
	}
	else
	{
		return 0;
	}
}


double ideaEffectMapper::GetArmyTechFromIdeas(const map<string, int>& ideas)
{
	double ideaEffect = 0.0;
	for (auto idea: ideas)
	{
		auto techIdea = armyTechIdeas.find(idea.first);
		if (techIdea != armyTechIdeas.end())
		{
			ideaEffect += techIdea->second * idea.second;
		}
	}

	return ideaEffect;
}


double ideaEffectMapper::GetCommerceTechFromIdeas(const map<string, int>& ideas)
{
	double ideaEffect = 0.0;
	for (auto idea: ideas)
	{
		auto techIdea = commerceTechIdeas.find(idea.first);
		if (techIdea != commerceTechIdeas.end())
		{
			ideaEffect += techIdea->second * idea.second;
		}
	}

	return ideaEffect;
}


double ideaEffectMapper::GetCultureTechFromIdeas(const map<string, int>& ideas)
{
	double ideaEffect = 0.0;
	for (auto idea: ideas)
	{
		auto techIdea = cultureTechIdeas.find(idea.first);
		if (techIdea != cultureTechIdeas.end())
		{
			ideaEffect += techIdea->second * idea.second;
		}
	}

	return ideaEffect;
}


double ideaEffectMapper::GetIndustryTechFromIdeas(const map<string, int>& ideas)
{
	double ideaEffect = 0.0;
	for (auto idea: ideas)
	{
		auto techIdea = industryTechIdeas.find(idea.first);
		if (techIdea != industryTechIdeas.end())
		{
			ideaEffect += techIdea->second * idea.second;
		}
	}

	return ideaEffect;
}


double ideaEffectMapper::GetNavyTechFromIdeas(const map<string, int>& ideas)
{
	double ideaEffect = 0.0;
	for (auto idea: ideas)
	{
		auto techIdea = navyTechIdeas.find(idea.first);
		if (techIdea != navyTechIdeas.end())
		{
			ideaEffect += techIdea->second * idea.second;
		}
	}

	return ideaEffect;
}