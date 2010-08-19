#include "DateParser.h" 

int DateParser::currYear = 0;
int DateParser::currMonth = 0;
int DateParser::currDay = 0;

void setYear (unsigned int y) {DateParser::currYear = y;}
void setMonth (unsigned int m) {DateParser::currMonth = m;}
void setDay (unsigned int d) {DateParser::currDay = d;}
