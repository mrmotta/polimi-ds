#ifndef LEADERLESS_DATASTORE_CLIENTOPERATION_H
#define LEADERLESS_DATASTORE_CLIENTOPERATION_H

#include <regex>
#include <string>
#include <regex>
#include <vector>

class ClientOperation {

    public:
    typedef enum {GET, PUT, EXIT, WRONG} ClientOperationType;

    private:
    ClientOperationType type = WRONG;
    std::string key;
    static const std::regex regexExit;
    static const std::regex regexGet;
    static const std::regex regexPut;
    std::string value;

    public:
    void parse(std::string &input);

    std::string getKey() const;
    ClientOperationType getType() const;
    std::string getValue() const;

    private:
    static std::vector<std::string> split(std::string &input);
};


#endif
