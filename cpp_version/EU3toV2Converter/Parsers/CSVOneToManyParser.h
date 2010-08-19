// ****************************************************************************
// *                                                                          *
// *		     EU3 to Victoria 2 conversion project                     *
// *                                                                          *
// ****************************************************************************


#pragma once

#ifndef CSV_ONETOMANY_PARSER_H
#define CSV_ONETOMANY_PARSER_H

///////////////////////////////////////////////////////////////////////////////
// Class Definition
///////////////////////////////////////////////////////////////////////////////
class CSVOneToManyParser
{
public:
   CSVOneToManyParser() {};
   ~CSVOneToManyParser() {};

   std::vector<oneToMany> ProcessFile(std::string& filename);

private:   

};

#endif // ndef CSV_ONETOMANY_PARSER_H
