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



#ifndef HoI4ARMY_H_
#define HoI4ARMY_H_



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


class HoI4RegimentType
{
	public:
		HoI4RegimentType() : force_type(land), max_strength(1), name(L"INVALID"), practicalBonus(L""), practicalBonusFactor(0.0) {}; // req'd for map<>
		HoI4RegimentType(wstring type);

		ForceType				getForceType() const					{ return force_type; }
		wstring					getName() const						{ return name; }
		int						getMaxStrength() const				{ return max_strength; }
		const set<wstring>&	getUsableBy() const					{ return usableBy; }
		wstring					getPracticalBonus() const			{ return practicalBonus; }
		double					getPracticalBonusFactor() const	{ return practicalBonusFactor; }

		bool operator < (const HoI4RegimentType& rhs) const { return this->name < rhs.name; }; // multiset requires this

	private:
		ForceType		force_type;
		wstring			name;
		int				max_strength;
		set<wstring>	usableBy;
		wstring			practicalBonus;
		double			practicalBonusFactor;
};


class HoI4Regiment // also Ship, Wing
{
	public:
		void	output(FILE* out, int indentlevel) const;
		void	outputIntoProductionQueue(FILE* out) const;

		void	setName(wstring _name)									{ name = _name; }
		void	setType(HoI4RegimentType _type)						{ type = _type; }
		void	setHistoricalModel(unsigned _historicalModel)	{ historicalModel = _historicalModel; }
		void	setReserve(bool _reserve)								{ reserve = _reserve; }

		ForceType					getForceType() const	{ return type.getForceType(); }
		const HoI4RegimentType&	getType() const		{ return type; }
		bool							isReserve() const		{ return reserve; }

	private:
		wstring				name;
		HoI4RegimentType	type;
		bool					reserve;
		unsigned				historicalModel; // Tech level
};


class HoI4RegGroup // also Navy, Air
{
	public:
		HoI4RegGroup();

		void	output(FILE* out, int indentlevel = 0) const;
		void	outputIntoProductionQueue(FILE* out, const wstring& tag) const;
		
		static void	resetRegGroupNameCounts();
		static void	resetHQCounts();
		void			setName();
		bool			addRegiment(HoI4Regiment reg, bool allowPromote);
		bool			addChild(HoI4RegGroup group, bool allowPromote);
		void			createHQs(HoI4RegimentType hqType);
		void			undoPracticalAddition(map<wstring, double>& practicals) const;
		int			size() const;

		void	setName(wstring _name)									{ name = _name; }
		void	setLocation(int provinceID)							{ location = provinceID; }
		void	setForceType(ForceType _type)							{ force_type = _type; }
		void	setAtSea(int atSea)										{ at_sea = atSea; }
		void	setLeaderID(int id)										{ leaderID = id; }
		void	setProductionQueue(const bool _productionQueue)	{ productionQueue = _productionQueue; }
		void	setCommandLevel(CommandLevel lvl)					{ command_level = lvl; }

		wstring			getName() const					{ return name; }
		ForceType		getForceType() const				{ return force_type; }
		bool				isEmpty() const					{ return (regiments.size() == 0 && children.size() == 0); }
		bool				getProductionQueue() const		{ return productionQueue; }
		CommandLevel	getCommandLevel() const			{ return command_level; }

	private:
		HoI4RegGroup			createChild();

		wstring					name;
		int						location;
		vector<HoI4Regiment>	regiments;
		ForceType				force_type;
		int						at_sea;
		int						leaderID;
		vector<HoI4RegGroup>	children;
		CommandLevel			command_level;
		bool						productionQueue;
};



#endif