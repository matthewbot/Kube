#include "bindings.h"
#include <sstream>

ToCValueException::ToCValueException(int argn, std::type_index expected, std::string msg) :
    argn(argn),
    expected(expected),
    msg(msg) { }

const char *ToCValueException::what() const throw() { return msg.c_str(); }
