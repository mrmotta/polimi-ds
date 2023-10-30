//
// Created by Niccolò Nicolosi on 07/03/23.
//

#ifndef LEADERLESS_DATASTORE_CLIENT_H
#define LEADERLESS_DATASTORE_CLIENT_H


#include "ClientConfig.h"
#include "ClientConnectionHandler.h"

class Client {

private:
    ClientConnectionHandler connectionHandler;

public:
    explicit Client(ClientConfig &config);

    void get(const std::string &key);
    void put(const std::string &key, const std::string &value);
};


#endif //LEADERLESS_DATASTORE_CLIENT_H
