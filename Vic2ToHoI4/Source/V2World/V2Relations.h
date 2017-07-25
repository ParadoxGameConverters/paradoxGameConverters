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



#ifndef V2RELATIONS_H_
#define V2RELATIONS_H_



#include "Date.h"



class Object;



class V2Relations
{
	public:
		V2Relations(shared_ptr<Object> relationsObj);

		string getTag() const { return tag; }
		int getRelations() const { return value; }
		int getLevel() const { return level; }

		bool hasMilitaryAccess() const { return militaryAccess; }
		date getDiplomatLastSent() const { return lastSentDiplomat; }
		date getLastWar() const { return lastWar; }
		date getTruceUntil() const { return truceUntil; }

	private:
		void setValue(shared_ptr<Object> relationsObj);
		void setMilitaryAccess(shared_ptr<Object> relationsObj);
		void setLastDiplomat(shared_ptr<Object> relationsObj);
		void setLastWar(shared_ptr<Object> relationsObj);
		void setTruce(shared_ptr<Object> relationsObj);
		void setLevel(shared_ptr<Object> relationsObj);

		string tag;
		int value;
		int level;

		bool militaryAccess;
		date lastSentDiplomat;
		date lastWar;
		date truceUntil;
};



#endif // V2RELATIONS_H_