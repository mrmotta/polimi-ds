//
// Created by Pierluigi Negro on 3/14/23.
//

#ifndef LEADERLESS_DATASTORE_REGEXMISMATCHEXCEPTION_H
#define LEADERLESS_DATASTORE_REGEXMISMATCHEXCEPTION_H


#include <stdexcept>
#include <string>

class RegexMismatchException : public std::runtime_error {

public:
    RegexMismatchException();
    explicit RegexMismatchException(const std::string& message);
};


#endif //LEADERLESS_DATASTORE_REGEXMISMATCHEXCEPTION_H
