#ifndef LEADERLESS_DATASTORE_REGEX_H
#define LEADERLESS_DATASTORE_REGEX_H

#include <regex>

class Regex {

    // General
    public:
    static const std::regex split;

    // Common
    public:
    static const std::regex ack;
    static const std::regex nack;
    static const std::regex value;

    // Server
    public:
    static const std::regex fetch;
    static const std::regex fetchedValue;
    static const std::regex fetchedValueNull;
    static const std::regex get;
    static const std::regex globalAbort;
    static const std::regex globalCommit;
    static const std::regex prepare;
    static const std::regex put;
    static const std::regex repair;
    static const std::regex sync;
    static const std::regex syncEnd;
    static const std::regex syncStart;
    static const std::regex update;
    static const std::regex updateEnd;
    static const std::regex updateStart;
    static const std::regex voteAbort;
    static const std::regex voteCommit;
};

#endif
