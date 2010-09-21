#ifndef OBJECT_HH
#define OBJECT_HH

#include <map> 
#include <vector>
#include <string> 
#include <iostream>

class Object {
  friend std::ostream& operator<< (std::ostream& o, const Object& i);

public:
  Object (std::string k);
  ~Object (); 
  Object (Object* other);

  static bool debug;

  void setValue (Object* val);
  void setValue (std::string val);
  void setValue (std::vector<Object*> val);
  std::string getKey () {return key;} 
  std::vector<std::string> getKeys (); 
  std::vector<Object*> getValue (std::string key) const;
  std::string getLeaf () const {return strVal;}
  std::string getLeaf (std::string leaf) const;
  std::vector<Object*> getLeaves () {return objects;}
  void removeObject (Object* target); 
  void addObject (Object* target); 
  void addObjectAfter(Object* target, std::string key);
  void setLeaf (std::string k, std::string value); 
  void unsetValue (std::string val);
  void keyCount ();
  void keyCount (std::map<std::string, int>& counter);
  void setObjList (bool l = true) {isObjList = l;}
  std::string getToken (int index); 
  std::vector<std::string> getTokens() { return tokens; }
  int numTokens (); 
  void addToList (std::string val); 
  void printTopLevel ();
  inline bool isLeaf () {return leaf;}
  double safeGetFloat (std::string k, double def = 0);
  std::string safeGetString (std::string k, std::string def = ""); 
  int safeGetInt (std::string k, int def = 0);
  Object* safeGetObject (std::string k, Object* def = 0);
  std::string toString () const; 
  
private:
  std::string key;
  std::string strVal;
  std::vector<Object*> objects;     
  bool leaf; 
  bool isObjList;
  std::vector<std::string> tokens; // For use in lists. 
};

extern std::ostream& operator<< (std::ostream& os, const Object& i);
extern Object* br; 
extern void setVal (std::string name, std::string val, Object* branch = 0);
extern void setInt (std::string name, int val, Object* branch = 0);
extern void setFlt (std::string name, double val, Object* branch = 0);
typedef std::vector<Object*>::iterator objiter;
typedef std::vector<Object*> objvec; 
typedef std::map<std::string, Object*> stobmap;
typedef std::map<std::string, std::string> ststmap;
typedef std::map<Object*, Object*> obobmap;
extern Object* processFile (char* filename, bool includes = false);
extern Object* processFile (std::string filename, bool includes = false);


#endif
