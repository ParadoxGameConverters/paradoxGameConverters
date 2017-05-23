#ifndef PARADOX_PARSER_GENERIC_H
#define PARADOX_PARSER_GENERIC_H

#include <string>
#include <iostream>

#include "Encoding.h"
#include "ConversionFacet.h"
#include "Object.h"

namespace parser_generic{

	Object *parse(std::basic_istream<wchar_t> &input);

	Object *parse(const std::string &file_path, Encoding file_encoding);

	Object *parseUTF_8(const std::string &file_path);

	Object *parseISO_8859_15(const std::string &path);

}

#endif
