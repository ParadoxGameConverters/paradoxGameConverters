#include "Object.h"
#include "Parser.h"
#include <sstream> 

bool Object::debug = false; 

Object::Object (std::string k) :
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
  for (std::vector<Object*>::iterator i = other->objects.begin(); i != other->objects.end(); ++i) {
    objects.push_back(new Object(*i)); 
  }
}

void Object::setValue (std::string val) {
  strVal = val; 
  leaf = true; 
}

void Object::setValue (Object* val) {
  objects.push_back(val); 
  leaf = false; 
}

void Object::unsetValue (std::string val) {
  for (unsigned int i = 0; i < objects.size(); ++i) {
    if (objects[i]->getKey() != val) continue; 
    objects[i] = objects.back(); 
    objects.pop_back(); 
  }
}

void Object::setLeaf (std::string key, std::string val) {
  Object* leaf = new Object(key); 
  leaf->setValue(val); 
  setValue(leaf); 
}

void Object::setValue (std::vector<Object*> val) {
  objects = val; 
}

void Object::addToList(std::string val) {
  isObjList = true;
  if (strVal.size() > 0)
   strVal += " ";
  strVal += val; 
  tokens.push_back(val); 
}

std::vector<Object*> Object::getValue (std::string key) const {
  std::vector<Object*> ret; 
  for (std::vector<Object*>::const_iterator i = objects.begin(); i != objects.end(); ++i) {
    if ((*i)->getKey() != key) continue;
    ret.push_back(*i); 
  }
  return ret; 
}

std::string Object::getToken (int index) {
  if (!isObjList) return "";
  if (index >= (int) tokens.size()) return ""; 
  if (index < 0) return ""; 
  return tokens[index]; 
}

int Object::numTokens () {
  if (!isObjList) return 0;
  return tokens.size(); 
}

std::vector<std::string> Object::getKeys () {
  std::vector<std::string> ret;
  for (std::vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
    std::string curr = (*i)->getKey();
    if (std::find(ret.begin(), ret.end(), curr) != ret.end()) continue;
    ret.push_back(curr);
  }
  return ret; 
}

std::string Object::getLeaf (std::string leaf) const {
  std::vector<Object*> leaves = getValue(leaf); 
  if (0 == leaves.size()) {
    std::cout << "Error: Cannot find leaf " << leaf << " in object " << std::endl << *this;
    assert(leaves.size()); 
  }
  return leaves[0]->getLeaf(); 
}

std::ostream& operator<< (std::ostream& os, const Object& obj) {
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

  if (&obj != Parser::topLevel) {
    os << obj.key << "=\n";
    for (int i = 0; i < indent; i++) {
       os << "\t"; 
    }
    os << "{\n";
    indent++;
  }
  for (std::vector<Object*>::const_iterator i = obj.objects.begin(); i != obj.objects.end(); ++i) {
    os << *(*i); 
  }
  if (&obj != Parser::topLevel) {
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
    std::cout << key << " : 1\n";
    return;
  }
  
  std::map<std::string, int> refCount;
  keyCount(refCount); 
  std::vector<std::pair<std::string, int> > sortedCount; 
  for (std::map<std::string, int>::iterator i = refCount.begin(); i != refCount.end(); ++i) {
    std::pair<std::string, int> curr((*i).first, (*i).second); 
    if (2 > curr.second) continue; 
    if ((0 == sortedCount.size()) || (curr.second <= sortedCount.back().second)) {
      sortedCount.push_back(curr); 
      continue; 
    }

    for (std::vector<std::pair<std::string, int> >::iterator j = sortedCount.begin(); j != sortedCount.end(); ++j) {
      if (curr.second < (*j).second) continue; 
      sortedCount.insert(j, 1, curr);
      break; 
    }
  }
  
  for (std::vector<std::pair<std::string, int> >::iterator j = sortedCount.begin(); j != sortedCount.end(); ++j) {
    std::cout << (*j).first << " : " << (*j).second << "\n"; 
  }

}

void Object::keyCount (std::map<std::string, int>& counter) {
  for (std::vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
    counter[(*i)->key]++;
    if ((*i)->leaf) continue; 
    (*i)->keyCount(counter); 
  }
}

void Object::printTopLevel () {
  for (std::vector<Object*>::iterator i = objects.begin(); i != objects.end(); ++i) {
    std::cout << (*i)->key << std::endl; 
  }
}
void Object::removeObject (Object* target) {
  std::vector<Object*>::iterator pos = std::find(objects.begin(), objects.end(), target);
  if (pos == objects.end()) return; 
  objects.erase(pos); 
}

void Object::addObject (Object* target) {
   objects.push_back(target); 
}


Object* br = 0;
void setVal (std::string name, std::string val, Object* branch) {
  if ((branch) && (br != branch)) br = branch; 
  Object* b = new Object(name);
  b->setValue(val); 
  br->setValue(b); 
}

void setInt (std::string name, int val, Object* branch) {
  if ((branch) && (br != branch)) br = branch; 
  static char strbuffer[1000];
  sprintf(strbuffer, "%i", val); 
  Object* b = new Object(name);
  b->setValue(strbuffer); 
  br->setValue(b); 
}

void setFlt (std::string name, double val, Object* branch) {
  if ((branch) && (br != branch)) br = branch; 
  static char strbuffer[1000];
  sprintf(strbuffer, "%.3f", val); 
  Object* b = new Object(name);
  b->setValue(strbuffer); 
  br->setValue(b); 
}

void processIncludes (Object* ret) {
  objvec empty; 
  objvec includes = ret->getValue("include"); 
  for (objiter i = includes.begin(); i != includes.end(); ++i) {
    Object* temp = processFile((*i)->getLeaf()); 
    objvec nleaves = temp->getLeaves(); 
    for (objiter nl = nleaves.begin(); nl != nleaves.end(); ++nl) {
      ret->setValue(*nl); 
    }
    temp->setValue(empty); 
    delete temp; 
  }
}

Object* processFile (std::string filename, bool includes) {
  Parser::topLevel = new Object("toplevel"); 
  std::ifstream read;
  read.open(filename.c_str());
  readFile(read);
  read.close();
  Object* ret = Parser::topLevel; 
  if (includes) {
    processIncludes(ret); 
    Parser::topLevel = ret; 
  }
  return ret; 
}

Object* processFile (char* filename, bool includes) {
  Parser::topLevel = new Object("toplevel"); 
  std::ifstream read;
  read.open(filename);
  readFile(read);
  read.close();
  Object* ret = Parser::topLevel; 
  if (includes) {
    processIncludes(ret); 
    Parser::topLevel = ret; 
  }
  return ret;  
}

double Object::safeGetFloat(std::string k, double def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def;
  return atof(vec[0]->getLeaf().c_str());
}

std::string Object::safeGetString(std::string k, std::string def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def;
  return vec[0]->getLeaf(); 
}

int Object::safeGetInt(std::string k, int def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def;
  return atoi(vec[0]->getLeaf().c_str());
}

Object* Object::safeGetObject(std::string k, Object* def) {
  objvec vec = getValue(k); 
  if (0 == vec.size()) return def; 
  return vec[0]; 
}


std::string Object::toString () const {
  std::ostringstream blah;
  blah << *(this); 
  return blah.str(); 
}

