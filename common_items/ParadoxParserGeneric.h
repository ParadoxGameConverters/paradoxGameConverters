#ifndef PARADOX_PARSER_GENERIC_H
#define PARADOX_PARSER_GENERIC_H

#include <string>
#include <iostream>

#include "Encoding.h"
#include "Object.h"

namespace parser_generic{

	/*
	Parses the input stream and returns an object tree or nullptr if the stream could not be parsed
	*/
	Object *parse(std::basic_istream<wchar_t> &input);

	/*
	Parses the file pointed to by the path, assuming the specified encoding
	*/
	Object *parse(const std::string &file_path, Encoding file_encoding);

	/*
	Parses the file pointed to by the path, assuming UTF-8 encoding
	*/
	Object *parseUTF_8(const std::string &file_path);
	
	/*
	Parses the file pointed to by the path, assuming ISO 8859 15 encoding
	*/
	Object *parseISO_8859_15(const std::string &path);

}

#endif
