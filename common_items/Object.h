/*Copyright (c) 2018 The Paradox Game Converters Project

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



class Object
{
	  friend std::ostream& operator<< (std::ostream& o, const Object& i);

	public:
	  explicit Object(const std::string& k): key(k), strVal(), objects(), leaf(false), isObjList(false), tokens() {};
	  ~Object(); 
	  explicit Object(std::shared_ptr<Object> other);

	  std::string getKey() const { return key; }
	  std::string getLeaf() const { return strVal; }
	  std::vector<std::shared_ptr<Object>> getLeaves() { return objects; }
	  std::vector<std::string> getTokens() { return tokens; }
	  inline bool isLeaf() { return leaf; }

	  void setObjList(const bool l = true) { isObjList = l; }

	  std::optional<std::string> getLeaf(const std::string& leaf) const;
	  std::vector<std::string> getKeys();
	  std::vector<std::shared_ptr<Object>> getValue(const std::string& key) const;
	  std::optional<std::string> getToken(int index);
	  int numTokens();
	  double safeGetFloat(const std::string& k, double def = 0.0);
	  std::string safeGetString(const std::string& k, std::string def = "");
	  int safeGetInt(const std::string& k, int def = 0);
	  std::shared_ptr<Object> safeGetObject(const std::string& k, std::shared_ptr<Object> def = {});
	  std::vector<std::string> safeGetTokens(const std::string& k);
	  std::string toString() const;

	  void setValue(std::shared_ptr<Object> val);
	  void setValue(const std::string& val);
	  void setValue(const std::vector<std::shared_ptr<Object>>& val);
	  void removeObject(std::shared_ptr<Object> target); 
	  void addObject(std::shared_ptr<Object> target); 
	  void addObjectAfter(std::shared_ptr<Object> target, const std::string& key);
	  void setLeaf(std::string k, std::string value); 
	  void unsetValue(const std::string& val);
	  void keyCount();
	  void keyCount(std::map<std::string, int>& counter);
	  void addToList(std::string val); 
	  void addToList(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end);
	  void printTopLevel();
  
	private:
		std::string key;						// the higher level or LHS key for this object
		std::string strVal;					// the textual value for this object
		std::vector<std::shared_ptr<Object>> objects;		// any sub-objects
		bool leaf;						// whether or not this is a leaf object
		bool isObjList;					// whether or not this is an object list object
		std::vector<std::string> tokens;		// The tokens if this is a list object 
};


extern std::ostream& operator<< (std::ostream& os, const Object& i);
extern std::shared_ptr<Object> br; 
extern void setVal (std::string name, const std::string& val, std::shared_ptr<Object> branch = {});
extern void setInt (std::string name, int val, std::shared_ptr<Object> branch = {});
extern void setFlt (std::string name, double val, std::shared_ptr<Object> branch = {});
typedef std::vector<std::shared_ptr<Object>>::iterator objiter;
typedef std::vector<std::shared_ptr<Object>> objvec; 
typedef std::map<std::string, std::shared_ptr<Object>> stobmap;
typedef std::map<std::string, std::string> ststmap;
typedef std::map<std::shared_ptr<Object>, std::shared_ptr<Object>> obobmap;



#endif	// OBJECT_H
