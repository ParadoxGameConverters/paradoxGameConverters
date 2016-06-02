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
using namespace std;


class Object {
  friend wostream& operator<< (wostream& o, const Object& i);
  friend ostream& operator<< (ostream& o, const Object& i);

public:
  Object (wstring k);
  ~Object (); 
  Object (Object* other);

  void setValue (Object* val);
  void setValue (wstring val);
  void setValue (vector<Object*> val);
  wstring getKey () {return key;} 
  vector<wstring> getKeys (); 
  vector<Object*> getValue (wstring key) const;
  wstring getLeaf () const {return strVal;}
  wstring getLeaf (wstring leaf) const;
  vector<Object*> getLeaves () {return objects;}
  void removeObject (Object* target); 
  void addObject (Object* target); 
  void addObjectAfter(Object* target, wstring key);
  void setLeaf (wstring k, wstring value); 
  void unsetValue (wstring val);
  void keyCount ();
  void keyCount (map<wstring, int>& counter);
  void setObjList (const bool l = true) {isObjList = l;}
  wstring getToken (int index); 
  vector<wstring> getTokens() { return tokens; }
  int numTokens (); 
  void addToList (wstring val); 
  void addToList (vector<wstring>::iterator begin, vector<wstring>::iterator end);
  void printTopLevel ();
  inline bool isLeaf () {return leaf;}
  double safeGetFloat (wstring k, double def = 0);
  wstring safeGetString (wstring k, wstring def = L""); 
  int safeGetInt (wstring k, int def = 0);
  Object* safeGetObject (wstring k, Object* def = 0);
  wstring toString () const; 
  
private:
  wstring key;						// the higher level or LHS key for this object
  wstring strVal;					// the textual value for this object
  vector<Object*> objects;		// any sub-objects
  bool leaf;						// whether or not this is a leaf object
  bool isObjList;					// whether or not this is an object list object
  vector<wstring> tokens;		// The tokens if this is a list object 
};

extern wostream& operator<< (wostream& os, const Object& i);
extern Object* br; 
extern void setVal (wstring name, wstring val, Object* branch = 0);
extern void setInt (wstring name, int val, Object* branch = 0);
extern void setFlt (wstring name, double val, Object* branch = 0);
typedef vector<Object*>::iterator objiter;
typedef vector<Object*> objvec; 
typedef map<wstring, Object*> stobmap;
typedef map<wstring, wstring> ststmap;
typedef map<Object*, Object*> obobmap;

#endif	// OBJECT_H
