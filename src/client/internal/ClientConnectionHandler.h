//
// Created by Niccolò Nicolosi on 06/03/23.
//

#ifndef LEADERLESS_DATASTORE_CLIENTCONNECTIONHANDLER_H
#define LEADERLESS_DATASTORE_CLIENTCONNECTIONHANDLER_H


#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#endif
#include <string>
#include <vector>
#include <sstream>
#include "../../common/Message.h"

class ClientConnectionHandler {

private:
    static const int bufferSize = 256;

    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;
    #endif
    std::string bufferContent;
    int currReplicaSocket;
    char messageBuffer[bufferSize];
    std::vector<std::string> replicaAddresses;
    std::string partialMsg;

public:
    explicit ClientConnectionHandler(std::vector<std::string> replicaAddresses);
    ~ClientConnectionHandler();

    bool connectToReplica();
    void disconnect() const;
    void send(const Message &msg) const;
    void receive();
};


#endif //LEADERLESS_DATASTORE_CLIENTCONNECTIONHANDLER_H
