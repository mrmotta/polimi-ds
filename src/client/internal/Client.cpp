//
// Created by Niccolò Nicolosi on 07/03/23.
//

#include <iostream>
#include "../ClientOperation.h"
#include "../../common/Message.h"
#include "Client.h"
#include "ClientMessage.h"

using namespace std;

Client::Client(ClientConfig &config)
: connectionHandler(config.getServers()) {}

void Client::get(const std::string &key) {

    if (connectionHandler.connectToReplica()) {
        try {
            connectionHandler.send(ClientMessage::ClientGet(key));
            connectionHandler.receive();
        } catch (...) {
            cout << "An unexpected error occurred while communicating with the server. Please, try again later." << endl;
        }
        connectionHandler.disconnect();
    }
}

void Client::put(const std::string &key, const std::string &value) {

    if (connectionHandler.connectToReplica()) {
        try {
            connectionHandler.send(ClientMessage::ClientPut(key, value));
            connectionHandler.receive();
        } catch (...) {
            cout << "An unexpected error occurred while communicating with the server. Please, try again later." << endl;
        }
        connectionHandler.disconnect();
    }
}
