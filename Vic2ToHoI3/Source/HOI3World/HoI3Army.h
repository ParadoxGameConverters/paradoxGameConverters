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



#ifndef HOI3ARMY_H_
#define HOI3ARMY_H_



#include <io.h>
#include <string>
#include <map>
#include <set>
#include <vector>
using namespace std;



enum ForceType
{
	land,
	navy,
	air
};


enum CommandLevel
{
	division,
	corps,
	army,
	armygroup,
	theatre
};


class HoI3RegimentType
{
	public:
		HoI3RegimentType() : force_type(land), max_strength(1), name("INVALID"), practicalBonus(""), practicalBonusFactor(0.0) {}; // req'd for map<>
		HoI3RegimentType(string type);

		ForceType				getForceType() const					{ return force_type; }
		string					getName() const						{ return name; }
		int						getMaxStrength() const				{ return max_strength; }
		const set<string>&	getUsableBy() const					{ return usableBy; }
		string					getPracticalBonus() const			{ return practicalBonus; }
		double					getPracticalBonusFactor() const	{ return practicalBonusFactor; }

		bool operator < (const HoI3RegimentType& rhs) const { return this->name < rhs.name; }; // multiset requires this

	private:
		ForceType	force_type;
		string		name;
		int			max_strength;
		set<string>	usableBy;
		string		practicalBonus;
		double		practicalBonusFactor;
};


class HoI3Regiment // also Ship, Wing
{
	public:
		void	output(FILE* out, int indentlevel) const;
		void	outputIntoProductionQueue(FILE* out) const;

		void	setName(string _name)									{ name = _name; }
		void	setType(HoI3RegimentType _type)						{ type = _type; }
		void	setHistoricalModel(unsigned _historicalModel)	{ historicalModel = _historicalModel; }
		void	setReserve(bool _reserve)								{ reserve = _reserve; }

		ForceType					getForceType() const	{ return type.getForceType(); }
		const HoI3RegimentType&	getType() const		{ return type; }
		bool							isReserve() const		{ return reserve; }

	private:
		string				name;
		HoI3RegimentType	type;
		bool					reserve;
		unsigned				historicalModel; // Tech level
};


class HoI3RegGroup // also Navy, Air
{
	public:
		HoI3RegGroup();

		void	output(FILE* out, int indentlevel = 0) const;
		void	outputIntoProductionQueue(FILE* out, const string& tag) const;
		
		static void	resetRegGroupNameCounts();
		static void	resetHQCounts();
		void			setName();
		bool			addRegiment(HoI3Regiment reg, bool allowPromote);
		void			addChild(HoI3RegGroup group);
		void			createHQs(HoI3RegimentType hqType);
		void			undoPracticalAddition(map<string, double>& practicals) const;
		int			size() const;

		void	setName(string _name)									{ name = _name; }
		void	setLocation(int provinceID)							{ location = provinceID; }
		void	setForceType(ForceType _type)							{ force_type = _type; }
		void	setAtSea(int atSea)										{ at_sea = atSea; }
		void	setLeaderID(int id)										{ leaderID = id; }
		void	setProductionQueue(const bool _productionQueue)	{ productionQueue = _productionQueue; }
		void	setCommandLevel(CommandLevel lvl)					{ command_level = lvl; }

		string		getName() const					{ return name; }
		ForceType	getForceType() const				{ return force_type; }
		bool			isEmpty() const					{ return (regiments.size() == 0 && children.size() == 0); }
		bool			getProductionQueue() const		{ return productionQueue; }

	private:
		HoI3RegGroup			createChild();

		string					name;
		int						location;
		vector<HoI3Regiment>	regiments;
		ForceType				force_type;
		int						at_sea;
		int						leaderID;
		vector<HoI3RegGroup>	children;
		CommandLevel			command_level;
		bool						productionQueue;
};



#endif