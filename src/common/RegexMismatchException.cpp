//
// Created by Pierluigi Negro on 3/14/23.
//

#include "RegexMismatchException.h"

using namespace std;

RegexMismatchException::RegexMismatchException()
: runtime_error("String does not match the specified regex") {}

RegexMismatchException::RegexMismatchException(const string& message)
: runtime_error(message) {}
