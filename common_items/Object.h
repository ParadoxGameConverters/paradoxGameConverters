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
#include <memory>
#include <optional>
#include <vector>
#include <string> 
using namespace std;



class Object
{
	  friend ostream& operator<< (ostream& o, const Object& i);

	public:
	  explicit Object(const string& k): key(k), strVal(), objects(), leaf(false), isObjList(false), tokens() {};
	  ~Object(); 
	  explicit Object(shared_ptr<Object> other);

	  string getKey() const { return key; }
	  string getLeaf() const { return strVal; }
	  vector<shared_ptr<Object>> getLeaves() { return objects; }
	  vector<string> getTokens() { return tokens; }
	  inline bool isLeaf() { return leaf; }

	  void setObjList(const bool l = true) { isObjList = l; }

	  optional<string> getLeaf(const string& leaf) const;
	  vector<string> getKeys();
	  vector<shared_ptr<Object>> getValue(const string& key) const;
	  optional<string> getToken(int index);
	  int numTokens();
	  double safeGetFloat(const string& k, double def = 0.0);
	  string safeGetString(const string& k, string def = "");
	  int safeGetInt(const string& k, int def = 0);
	  shared_ptr<Object> safeGetObject(const string& k, shared_ptr<Object> def = {});
	  vector<string> safeGetTokens(const string& k);
	  string toString() const;

	  void setValue(shared_ptr<Object> val);
	  void setValue(const string& val);
	  void setValue(const vector<shared_ptr<Object>>& val);
	  void removeObject(shared_ptr<Object> target); 
	  void addObject(shared_ptr<Object> target); 
	  void addObjectAfter(shared_ptr<Object> target, const string& key);
	  void setLeaf(string k, string value); 
	  void unsetValue(const string& val);
	  void keyCount();
	  void keyCount(map<string, int>& counter);
	  void addToList(string val); 
	  void addToList(vector<string>::iterator begin, vector<string>::iterator end);
	  void printTopLevel();
  
	private:
	  string key;						// the higher level or LHS key for this object
	  string strVal;					// the textual value for this object
	  vector<shared_ptr<Object>> objects;		// any sub-objects
	  bool leaf;						// whether or not this is a leaf object
	  bool isObjList;					// whether or not this is an object list object
	  vector<string> tokens;		// The tokens if this is a list object 
};


extern ostream& operator<< (ostream& os, const Object& i);
extern shared_ptr<Object> br; 
extern void setVal (string name, const string& val, shared_ptr<Object> branch = {});
extern void setInt (string name, int val, shared_ptr<Object> branch = {});
extern void setFlt (string name, double val, shared_ptr<Object> branch = {});
typedef vector<shared_ptr<Object>>::iterator objiter;
typedef vector<shared_ptr<Object>> objvec; 
typedef map<string, shared_ptr<Object>> stobmap;
typedef map<string, string> ststmap;
typedef map<shared_ptr<Object>, shared_ptr<Object>> obobmap;



#endif	// OBJECT_H
