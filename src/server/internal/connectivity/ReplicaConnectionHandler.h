//
// Created by Niccolò Nicolosi on 06/03/23.
//

#ifndef LEADERLESS_DATASTORE_REPLICACONNECTIONHANDLER_H
#define LEADERLESS_DATASTORE_REPLICACONNECTIONHANDLER_H


#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#else
    #include <netinet/tcp.h>
    #include <netinet/in.h>
#endif
#include <string>
#include <optional>
#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "../../../common/Message.h"

class Replica;
class Coordinator;
class Participant;

class ReplicaConnectionHandler {

public:
    class Connection {

    private:
        static const int bufferSize = 256;
        int socket;

    public:
        const std::string name;

        Connection(std::string name, int socket,
                   ReplicaConnectionHandler &parentHandler);
        ~Connection();

        void startReceiving();
        void receive();
        void send(const Message &msg) const;
        void close();
        bool isOpen() const;

        friend std::ostream &operator<< (std::ostream &os, const Connection &c) {

            return (os << "(address: " << c.name << ")");
        }

    private:
        ReplicaConnectionHandler &parentHandler;
        Replica &parentReplica;
        bool connectionOpen;
        char messageBuffer[bufferSize];
        std::string bufferContent, partialMsg;
        std::thread *receivingThread;
    };

private:
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;
    #endif
    Replica &parent;
    std::unordered_map<std::string, Connection*> connections;
    std::thread *acceptingThread;
    int acceptingSocket;
    bool openBool;

    void acceptConnection();

public:
    const int readQuorum, writeQuorum;
    const std::vector<std::string> replicaAddresses;

    explicit ReplicaConnectionHandler(Replica &parent, std::vector<std::string> replicaAddresses,
                                      int readQuorum, int writeQuorum);

    void open(bool async);
    void close();
    bool isOpen() const;
    std::optional<Connection*> connectTo(const std::string &addressString, std::string *error = nullptr);
    void send(int socket, const Message &msg) const;
    Replica &getParent();
};

#endif //LEADERLESS_DATASTORE_REPLICACONNECTIONHANDLER_H
