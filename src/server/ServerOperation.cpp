#include "ServerOperation.h"

using namespace std;

void ServerOperation::parse(string &line) {

    type = WRONG;

    if (line == "exit")
        type = EXIT;
    else if (line == "debug")
        type = DEBUG;
}

ServerOperation::ServerOperationType ServerOperation::getType() const {
    return type;
}
