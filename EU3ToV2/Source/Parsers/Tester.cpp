#include "Tester.h"
#include "Parser.h"
#include "Object.h"

int main (int argc, char** argv) {
  if (1 == argc) {
    Object* testObject = processFile("testfile.txt"); 
    std::cout << (*testObject) << std::endl; 
  }
  else {    
    Object* testObject = processFile(argv[1]); 
    assert(testObject); 
    std::cout << "Successfully parsed" << std::endl; 
  }
  return 0; 
}
