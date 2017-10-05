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



#ifndef HoI4ARMY_H_
#define HoI4ARMY_H_



#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>
using namespace std;



//enum ForceType
//{
//	land,
//	navy,
//	air
//};
//
//
//enum CommandLevel
//{
//	division,
//	corps,
//	army,
//	armygroup,
//	theatre
//};
//
//
//class oldHoI4RegimentType
//{
//	public:
//		oldHoI4RegimentType() : force_type(land), max_strength(1), name("INVALID"), practicalBonus(""), practicalBonusFactor(0.0) {}; // req'd for map<>
//		oldHoI4RegimentType(string type);
//
//		ForceType				getForceType() const					{ return force_type; }
//		string					getName() const						{ return name; }
//		int						getMaxStrength() const				{ return max_strength; }
//		const set<string>&	getUsableBy() const					{ return usableBy; }
//		string					getPracticalBonus() const			{ return practicalBonus; }
//		double					getPracticalBonusFactor() const	{ return practicalBonusFactor; }
//
//		bool operator < (const oldHoI4RegimentType& rhs) const { return this->name < rhs.name; }; // multiset requires this
//
//	private:
//		ForceType		force_type;
//		string			name;
//		int				max_strength;
//		set<string>	usableBy;
//		string			practicalBonus;
//		double			practicalBonusFactor;
//};
//
//
//class HoI4Regiment // also Ship, Wing
//{
//	public:
//		void	output(FILE* out, int indentlevel) const;
//		void	outputIntoProductionQueue(FILE* out) const;
//
//		void	setName(string _name)									{ name = _name; }
//		void	setType(oldHoI4RegimentType _type)					{ type = _type; }
//		void	setHistoricalModel(unsigned _historicalModel)	{ historicalModel = _historicalModel; }
//		void	setReserve(bool _reserve)								{ reserve = _reserve; }
//
//		ForceType						getForceType() const	{ return type.getForceType(); }
//		const oldHoI4RegimentType&	getType() const		{ return type; }
//		bool								isReserve() const		{ return reserve; }
//
//	private:
//		string					name;
//		oldHoI4RegimentType	type;
//		bool						reserve;
//		unsigned					historicalModel; // Tech level
//};
//
//
//class HoI4RegGroup // also Navy, Air
//{
//	public:
//		HoI4RegGroup();
//
//		void	output(FILE* out, int indentlevel = 0) const;
//		void	outputIntoProductionQueue(FILE* out, const string& tag) const;
//		
//		static void	resetRegGroupNameCounts();
//		static void	resetHQCounts();
//		void			setName();
//		bool			addRegiment(HoI4Regiment reg, bool allowPromote);
//		bool			addChild(HoI4RegGroup group, bool allowPromote);
//		void			createHQs(HoI4RegimentType hqType);
//		void			undoPracticalAddition(map<string, double>& practicals) const;
//		int			size() const;
//
//		void	setName(string _name)									{ name = _name; }
//		void	setLocation(int provinceID)							{ location = provinceID; }
//		void	setForceType(ForceType _type)							{ force_type = _type; }
//		void	setAtSea(int atSea)										{ at_sea = atSea; }
//		void	setLeaderID(int id)										{ leaderID = id; }
//		void	setProductionQueue(const bool _productionQueue)	{ productionQueue = _productionQueue; }
//		void	setCommandLevel(CommandLevel lvl)					{ command_level = lvl; }
//
//		string			getName() const					{ return name; }
//		ForceType		getForceType() const				{ return force_type; }
//		bool				isEmpty() const					{ return (regiments.size() == 0 && children.size() == 0); }
//		bool				getProductionQueue() const		{ return productionQueue; }
//		CommandLevel	getCommandLevel() const			{ return command_level; }
//
//	private:
//		HoI4RegGroup			createChild();
//
//		string					name;
//		int						location;
//		vector<HoI4Regiment>	regiments;
//		ForceType				force_type;
//		int						at_sea;
//		int						leaderID;
//		vector<HoI4RegGroup>	children;
//		CommandLevel			command_level;
//		bool						productionQueue;
//};


class HoI4RegimentType
{
	public:
		explicit HoI4RegimentType(const string& type, int x, int y);
		HoI4RegimentType(const HoI4RegimentType&) = default;

		const string getType() const { return type; }

		friend ostream& operator << (ostream& out, const HoI4RegimentType& regiment);

	private:
		HoI4RegimentType& operator=(const HoI4RegimentType&) = delete;

		string	type;
		int		x;
		int		y;
};


class HoI4DivisionTemplateType
{
	public:
		explicit HoI4DivisionTemplateType(const string& name);
		HoI4DivisionTemplateType(const HoI4DivisionTemplateType&) = default;
		HoI4DivisionTemplateType& operator=(const HoI4DivisionTemplateType&) = default;

		friend ostream& operator << (ostream& out, const HoI4DivisionTemplateType& rhs);

		void addRegiment(const HoI4RegimentType& newRegiment)			{ regiments.push_back(newRegiment); }
		void addSupportRegiment(const HoI4RegimentType& newRegiment)	{ supportRegiments.push_back(newRegiment); }

		string getName() const { return name; }

		vector<HoI4RegimentType> getRegiments() const { return regiments; }
		vector<HoI4RegimentType> getSupportRegiments() const { return supportRegiments; }

	private:
		string							name;
		vector<HoI4RegimentType>	regiments;
		vector<HoI4RegimentType>	supportRegiments;
};


class HoI4DivisionType
{
	public:
		HoI4DivisionType(const string& name, const string& type, int location);
		HoI4DivisionType(const HoI4DivisionType&) = default;

		friend ostream& operator << (ostream& out, const HoI4DivisionType&);

	private:
		HoI4DivisionType& operator=(const HoI4DivisionType&) = delete;

		string	name;
		string	type;
		int		location;
};


class HoI4UnitMap
{
	public: 
		HoI4UnitMap(const string& category, const string& type, const string& equipment, int size);
		HoI4UnitMap();
		HoI4UnitMap(const HoI4UnitMap&) = default;
		HoI4UnitMap& operator=(const HoI4UnitMap&) = default;

		string getCategory();
		string getType();
		string getEquipment();
		int getSize();

	private:
		string category;
		string type;
		string equipment;
		int size;
};



#endif // HoI4ARMY_H_