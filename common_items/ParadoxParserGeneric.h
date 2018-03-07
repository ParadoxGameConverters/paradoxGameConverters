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
	shared_ptr<Object> parse(std::basic_istream<wchar_t> &input);

	/*
	Parses the file pointed to by the path, assuming the specified encoding
	*/
	shared_ptr<Object> parseFile(const std::string &file_path, Encoding file_encoding);

	/*
	Parses the file pointed to by the path, assuming UTF-8 encoding
	*/
	shared_ptr<Object> parseUTF_8File(const std::string &file_path);
	
	/*
	Parses the file pointed to by the path, assuming ISO 8859 15 encoding
	*/
	shared_ptr<Object> parseISO_8859_15File(const std::string &path);

	/*
	Parses the provided stream, assuming UTF-8 encoding
	*/
	shared_ptr<Object> parseUTF_8(std::istream& input);

	/*
	Parses the provided stream, assuming ISO 8859 15 encoding
	*/
	shared_ptr<Object> parseISO_8859_15(std::istream& input);

	/*
	Parses the provided stream, assuming the specified encoding
	*/
	shared_ptr<Object> parseStream(std::istream& input, Encoding stream_encoding);

}

#endif
