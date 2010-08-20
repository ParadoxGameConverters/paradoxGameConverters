#include "InstructionsParser.h"

static varDefinition	curVar;
static ruleset		curRules;
static bool		processingVar;

static std::vector<varDefinition>   processedVars;
static std::vector<ruleset>	    processedRulesets;

const static std::string COUNTRYVAR = "COUNTRYVAR";
const static std::string PROVINCEVAR = "PROVINCEVAR";
const static std::string WORLDVAR = "WORLDVAR";
const static std::string RULESET = "RULESET";

void newVarOrRule (char const* first, char const* last)
{
   std::string key(first, last);

   // First, move current variable and ruleset to processed status
   if (curVar.name.size() > 0)
   {
      std::vector<instruction> blanks;

      processedVars.push_back(curVar);

      curVar.name = "";
      curVar.type = VAR_ILLEGAL;
      curVar.instructions = blanks;
   }

   if (curRules.rules.size() > 0)
   {
      processedRulesets.push_back(curRules);
      curRules.name = "";
      curRules.rules.empty();
   }

   if (key.compare(COUNTRYVAR) == 0)
   {
      processingVar = TRUE;
      curVar.type = VAR_COUNTRY;
   }
   else if (key.compare(PROVINCEVAR) == 0)
   {
      processingVar = TRUE;
      curVar.type = VAR_PROVINCE;
   }
   else if (key.compare(WORLDVAR) == 0)
   {
      processingVar = TRUE;
      curVar.type = VAR_WORLD;
   }
   else if (key.compare(RULESET) == 0)
   {
      processingVar = FALSE;      
   }
   else
   {
      // TODO: Error!
   }
}

void getVarRuleName (char const* first, char const* last)
{
   std::string key(first, last);

   if (processingVar)
   {
      curVar.name = key;
   }
   else
   {
      curRules.name = key;
   }
}


void parseInstruction (char const* first, char const* last)
{
   instruction instr;
   std::string key(first, last);
   std::vector<std::string> tokens = tokenize_str(key);

   // Skip empty strings
   if (tokens.size() < 1)
      return;

   // The first token is the type of action we want to do
   if (tokens[0].compare("=") == 0)
   {
      // Set the value from either flag or numeric constant
      // Is second token starting with $?            
      if ((tokens.size()>1) && (tokens[1].at(0) == '$'))
      {
	 // Add flag
	 instr.type = INS_SET_FLAG;
	 instr.strVal = tokens[1].substr(1);
	 instr.dblVal = 1.0;

	 if ((tokens.size()>2) && (atof(tokens[2].c_str()) != 0.0))
	    instr.dblVal = atof(tokens[2].c_str());
      }
      else if ((tokens.size()>1) && (atof(tokens[1].c_str()) != 0.0))
      {
	 // Add value
	 instr.type = INS_SET_VALUE;
	 instr.strVal = "";
	 instr.dblVal = atof(tokens[1].c_str());
      }
   }
   else if ((tokens[0].compare("+=") == 0) || (tokens[0].compare("+") == 0))
   {
      // Is second token starting with $?            
      if ((tokens.size()>1) && (tokens[1].at(0) == '$'))
      {
	 // Add flag
	 instr.type = INS_ADD_FLAG;
	 instr.strVal = tokens[1].substr(1);
	 instr.dblVal = 1.0;

	 if ((tokens.size()>2) && (atof(tokens[2].c_str()) != 0.0))
	    instr.dblVal = atof(tokens[2].c_str());
      }
      else if ((tokens.size()>1) && (atof(tokens[1].c_str()) != 0.0))
      {
	 // Add value
	 instr.type = INS_ADD_VALUE;
	 instr.strVal = "";
	 instr.dblVal = atof(tokens[1].c_str());
      }
   }

   curVar.instructions.push_back(instr);
}

void finish()
{
   processedVars.push_back(curVar);
   processedRulesets.push_back(curRules);
}

void readInsFile (std::ifstream& read)
{
   std::ostringstream data;

   while (read.good())
   {
      std::string buffer;
      std::getline(read, buffer);   

      string::size_type endPos = buffer.find_first_of("#", 0);

      if (string::npos != endPos)
      {
	 buffer = buffer.substr(0, endPos);	 
	 if (buffer.size() < 1)
	    continue;
      }

      data << buffer << "\n";
   }
   
   InstructionsParser p;
   BOOST_SPIRIT_DEBUG_GRAMMAR(p);
   boost::spirit::parse_info<> result = boost::spirit::parse(data.str().c_str(), p, boost::spirit::nothing_p);

   finish();
}

