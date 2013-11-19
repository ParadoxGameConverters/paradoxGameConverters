#ifndef OBJECT_H
#define OBJECT_H


#include <map> 
#include <vector>
#include <string> 
using namespace std;


class Object {
  friend ostream& operator<< (ostream& o, const Object& i);

public:
  Object (string k);
  ~Object (); 
  Object (Object* other);

  static bool debug;

  void setValue (Object* val);
  void setValue (string val);
  void setValue (vector<Object*> val);
  string getKey () {return key;} 
  vector<string> getKeys (); 
  vector<Object*> getValue (string key) const;
  string getLeaf () const {return strVal;}
  string getLeaf (string leaf) const;
  vector<Object*> getLeaves () {return objects;}
  void removeObject (Object* target); 
  void addObject (Object* target); 
  void addObjectAfter(Object* target, string key);
  void setLeaf (string k, string value); 
  void unsetValue (string val);
  void keyCount ();
  void keyCount (map<string, int>& counter);
  void setObjList (bool l = true) {isObjList = l;}
  string getToken (int index); 
  vector<string> getTokens() { return tokens; }
  int numTokens (); 
  void addToList (string val); 
  void addToList (vector<string>::iterator begin, vector<string>::iterator end);
  void printTopLevel ();
  inline bool isLeaf () {return leaf;}
  double safeGetFloat (string k, double def = 0);
  string safeGetString (string k, string def = ""); 
  int safeGetInt (string k, int def = 0);
  Object* safeGetObject (string k, Object* def = 0);
  string toString () const; 
  
private:
  string key;
  string strVal;
  vector<Object*> objects;     
  bool leaf; 
  bool isObjList;
  vector<string> tokens; // For use in lists. 
};

extern ostream& operator<< (ostream& os, const Object& i);
extern Object* br; 
extern void setVal (string name, string val, Object* branch = 0);
extern void setInt (string name, int val, Object* branch = 0);
extern void setFlt (string name, double val, Object* branch = 0);
typedef vector<Object*>::iterator objiter;
typedef vector<Object*> objvec; 
typedef map<string, Object*> stobmap;
typedef map<string, string> ststmap;
typedef map<Object*, Object*> obobmap;

#endif	// OBJECT_H
