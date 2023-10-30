#ifndef LEADERLESS_DATASTORE_SERVEROPERATION_H
#define LEADERLESS_DATASTORE_SERVEROPERATION_H

#include <vector>
#include <string>

class ServerOperation {

    public:
    typedef enum {EXIT, DEBUG, WRONG} ServerOperationType;

    private:
    ServerOperationType type = WRONG;

    public:
    void parse(std::string &input);

    ServerOperationType getType() const;
};

#endif
