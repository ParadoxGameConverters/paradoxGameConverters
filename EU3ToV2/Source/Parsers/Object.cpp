#include "Object.h"
#include "Parser.h"
#include <sstream> 
#include <fstream>
#include <algorithm>
#include <iostream>
#include <assert.h>

bool Object::debug = false; 

Object::Object (string k) :
  objects(),
  strVal(),
  leaf(false),
  isObjList(false)
{
  key = k; 
}

Object::~Object () {
  for (objiter i = objects.begin(); i != objects.end(); ++i) {
    delete (*i); 
  }
  if (br == this) br = 0; 
}

Object::Object (Object* other) :
  objects(),
  strVal(other->strVal),
  leaf(other->leaf), 
  isObjList(other->isObjList)
{
  key = other->key; 
  for (vector<Object*>::iterator i = other->objects.begin(); i != other->objects.end(); ++i) {
    objects.push_back(new Object(*i)); 
  }
}

void Object::setValue (string val) {
  strVal = val; 
  leaf = true; 
}

void Object::setValue (Object* val) {
  objects.push_back(val); 
  leaf = false; 
}

void Object::unsetValue (string val) {
  for (unsigned int i = 0; i < objects.size(); ++i) {
    if (objects[i]->getKey() != val) continue; 
    objects[i] = objects.back(); 
    objects.pop_back(); 
  }
}

void Object::setLeaf (string key, string val) {
  Object* leaf = new Object(key); 
  leaf->setValue(val); 
  setValue(leaf); 
}

void Object::setValue (vector<Object*> val) {
  objects = val; 
}

void Object::addToList(string val) {
  isObjList = true;
  if (strVal.size() > 0)
   strVal += " ";
  strVal += val; 
  tokens.push_back(val); 
}

void Object::addToList(vector<string>::iterator begin, vector<string>::iterator end)
{
  isObjList = true;
  for (vector<string>::iterator itr = begin; itr != end; ++itr)
  {
    if (strVal.size() > 0)
      strVal += " ";
    strVal += *itr; 
  }
  tokens.insert(tokens.end(), begin, end);
}

vector<Object*> Object::getValue (string key) const {
  vector<Object*> ret; 
  for (vector<Object*>::const_iterator i = objects.begin(); i != objects.end(); ++i) {
    if ((*i)->getKey() != key) continue;
    ret.push_back(*i); 
  }
  return ret; 
}

string Object::getToken (int index) {
  if (!isObjList) return "";
  if (index >= (int) tokens.size()) return ""; 
  if (index < 0) return ""; 
  return tokens[index]; 
}

int Object::numTokens () {
  if (!isObjList) return 0;
  return tokens.size(); 
}

vector<string> Object::getKeys () {
  vector<string> ret;
  for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
    string curr = (*i)->getKey();
    if (find(ret.begin(), ret.end(), curr) != ret.end()) continue;
    ret.push_back(curr);
  }
  return ret; 
}

string Object::getLeaf (string leaf) const {
  vector<Object*> leaves = getValue(leaf); 
  if (0 == leaves.size()) {
    cout << "Error: Cannot find leaf " << leaf << " in object " << endl << *this;
    assert(leaves.size()); 
  }
  return leaves[0]->getLeaf(); 
}

ostream& operator<< (ostream& os, const Object& obj) {
  static int indent = 0; 
  for (int i = 0; i < indent; i++) {
    os << "\t"; 
  }
  if (obj.leaf) {
    os << obj.key << "=" << obj.strVal << "\n"; 
    return os; 
  }
  if (obj.isObjList) {
     os << obj.key << "={" << obj.strVal << " }\n";
    return os; 
  }

  if (&obj != getTopLevel()) {
    os << obj.key << "=\n";
    for (int i = 0; i < indent; i++) {
       os << "\t"; 
    }
    os << "{\n";
    indent++;
  }
  for (vector<Object*>::const_iterator i = obj.objects.begin(); i != obj.objects.end(); ++i) {
    os << *(*i); 
  }
  if (&obj != getTopLevel()) {
    indent--; 
    for (int i = 0; i < indent; i++) {
      os << "\t"; 
    }
    os << "}\n";
  }
  return os; 
}

void Object::keyCount () {
  if (leaf) {
    cout << key << " : 1\n";
    return;
  }
  
  map<string, int> refCount;
  keyCount(refCount); 
  vector<pair<string, int> > sortedCount; 
  for (map<string, int>::iterator i = refCount.begin(); i != refCount.end(); ++i) {
    pair<string, int> curr((*i).first, (*i).second); 
    if (2 > curr.second) continue; 
    if ((0 == sortedCount.size()) || (curr.second <= sortedCount.back().second)) {
      sortedCount.push_back(curr); 
      continue; 
    }

    for (vector<pair<string, int> >::iterator j = sortedCount.begin(); j != sortedCount.end(); ++j) {
      if (curr.second < (*j).second) continue; 
      sortedCount.insert(j, 1, curr);
      break; 
    }
  }
  
  for (vector<pair<string, int> >::iterator j = sortedCount.begin(); j != sortedCount.end(); ++j) {
    cout << (*j).first << " : " << (*j).second << "\n"; 
  }

}

void Object::keyCount (map<string, int>& counter) {
  for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
    counter[(*i)->key]++;
    if ((*i)->leaf) continue; 
    (*i)->keyCount(counter); 
  }
}

void Object::printTopLevel () {
  for (vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
    cout << (*i)->key << endl; 
  }
}
void Object::removeObject (Object* target) {
  vector<Object*>::iterator pos = find(objects.begin(), objects.end(), target);
  if (pos == objects.end()) return; 
  objects.erase(pos); 
}

void Object::addObject (Object* target) {
   objects.push_back(target); 
}

void Object::addObjectAfter(Object* target, string key) 
{   
   vector<Object*>::iterator i;

   for (i = objects.begin(); i != objects.end(); ++i) 
   {
      if ((*i)->getKey() == key)
      {
	 objects.insert(i, target);
	 break;
      }           
   }

   if (i == objects.end())
   {
      objects.push_back(target); 
   }
}



Object* br = 0;
void setVal (string name, string val, Object* branch) {
  if ((branch) && (br != branch)) br = branch; 
  Object* b = new Object(name);
  b->setValue(val); 
  br->setValue(b); 
}

void setInt (string name, int val, Object* branch) {
  if ((branch) && (br != branch)) br = branch; 
  static char strbuffer[1000];
  sprintf_s(strbuffer, 1000, "%i", val); 
  Object* b = new Object(name);
  b->setValue(strbuffer); 
  br->setValue(b); 
}

void setFlt (string name, double val, Object* branch) {
  if ((branch) && (br != branch)) br = branch; 
  static char strbuffer[1000];
  sprintf_s(strbuffer, 1000, "%.3f", val); 
  Object* b = new Object(name);
  b->setValue(strbuffer); 
  br->setValue(b); 
}

double Object::safeGetFloat(string k, double def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def;
  return atof(vec[0]->getLeaf().c_str());
}

string Object::safeGetString(string k, string def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def;
  return vec[0]->getLeaf(); 
}

int Object::safeGetInt(string k, int def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def;
  return atoi(vec[0]->getLeaf().c_str());
}

Object* Object::safeGetObject(string k, Object* def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def; 
  return vec[0]; 
}


string Object::toString () const {
  ostringstream blah;
  blah << *(this); 
  return blah.str(); 
}

