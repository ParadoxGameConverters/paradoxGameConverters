/*Copyright (c) 2014 The Paradox Game Converters Project

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



/*Copyright (c) 2010 Rolf Andreassen

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



#ifndef OBJECT_H
#define OBJECT_H

#include <map>
#include <vector>
#include <string>
#include "IObject.h"
using namespace std;


class Object : public IObject{

public:
  Object (string k);
  ~Object ();
  Object (IObject* other);

  virtual string print() const;
  void setValue (Object* val);
  void setValue (string val);
  void setValue (vector<IObject*> val);
  virtual string getKey () {return key;}
  vector<string> getKeys ();
  virtual vector<IObject*> getValue (string key) const;
  virtual string getLeaf () const {return strVal;}
  virtual string getLeaf (string leaf) const;
  virtual string getTitle (string key) const;
  virtual string getLeafValueOrThisValue(string key) const;
  virtual string getStringOrDefault(string key, std::function<string(const IObject*)> valueGetter,
                                  std::function<string(const IObject*)> defaultValueGetter) const;
  virtual vector<IObject*> getLeaves () {return objects;}
  void removeObject (Object* target);
  void addObject (IObject* target);
  void addObjectAfter(Object* target, string key);
  void setLeaf (string k, string value);
  void unsetValue (string val);
  void keyCount ();
  virtual void keyCount (map<string, int>& counter);
  void setObjList (const bool l = true) {isObjList = l;}
  string getToken (int index);
  virtual vector<string> getTokens() { return tokens; }
  int numTokens ();
  void addToList (string val);
  void addToList (vector<string>::iterator begin, vector<string>::iterator end);
  void printTopLevel ();
  virtual inline bool isLeaf () {return leaf;}
  double safeGetFloat (string k, double def = 0);
  string safeGetString (string k, string def = "");
  int safeGetInt (string k, int def = 0);
  IObject* safeGetObject (string k, IObject* def = 0);
  string toString () const;

  virtual bool isList() const { return isObjList; }

private:
  string getTitleValue(const IObject* titleObj) const;

  string key;						// the higher level or LHS key for this object
  string strVal;					// the textual value for this object
  vector<IObject*> objects;		// any sub-objects
  bool leaf;						// whether or not this is a leaf object
  bool isObjList;					// whether or not this is an object list object
  vector<string> tokens;		// The tokens if this is a list object
};

extern Object* br;
extern void setVal (string name, string val, Object* branch = 0);
extern void setInt (string name, int val, Object* branch = 0);
extern void setFlt (string name, double val, Object* branch = 0);
typedef vector<IObject*>::iterator objiter;
typedef vector<IObject*> objvec;
typedef map<string, Object*> stobmap;
typedef map<string, string> ststmap;
typedef map<Object*, Object*> obobmap;

#endif	// OBJECT_H
