//
// Created by Niccolò Nicolosi on 06/03/23.
//

#if defined(_WIN32) || defined(_WIN64)
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <vector>
#include <utility>
#include "../Replica.h"
#include "ReplicaConnectionHandler.h"

using namespace std;

ReplicaConnectionHandler::ReplicaConnectionHandler(Replica &parent, vector<string> replicaAddresses,
                                                   int readQuorum, int writeQuorum)
: parent(parent), acceptingThread(), acceptingSocket(-1), openBool(false),
readQuorum(readQuorum), writeQuorum(writeQuorum), replicaAddresses(std::move(replicaAddresses))
#if defined(_WIN32) || defined(_WIN64)
, wsa({})
#endif
{}

/**
 * Starts the handler, creating a socket to accept new connections and binding it to parentAddr
 * Blocking method if async is set to false
 * @param true if the handler has to accept connections asynchronously
 * **/
void ReplicaConnectionHandler::open(bool async) {

    #if defined(_WIN32) || defined(_WIN64)
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
            throw runtime_error((stringstream() << "Could not initialize Windows' socket subsystem. Error Code: " << WSAGetLastError()).str());
    #endif

    acceptingSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (acceptingSocket < 0)
        throw runtime_error("Couldn't create socket");

    const int enable = 1;
    if (setsockopt(acceptingSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &enable, sizeof(int)) < 0)
        throw runtime_error("Couldn't set socket option SO_REUSEADDR");

    const sockaddr_in &parentAddr = parent.getAddress();
    if (::bind(acceptingSocket, (const struct sockaddr *) &parentAddr, sizeof(parentAddr)) < 0)
        throw runtime_error((stringstream() << "Couldn't bind socket, port:" << htons(parentAddr.sin_port)).str());

    if(listen(acceptingSocket, 32) < 0)
        throw runtime_error("Couldn't start listening");
    parent.log((stringstream() << "Listening for new connections").str());

    openBool = true;

    auto acceptingFunction = [this]() -> void {

        while (isOpen())
            acceptConnection();
        parent.log("Stopped accepting connections");
    };

    if (async)
        acceptingThread = new thread(acceptingFunction);
    else
        acceptingFunction();
}

/**
 * Accepts a new connection and inserts the resulting socket in clientSockets.
 * Blocking method
 * **/
void ReplicaConnectionHandler::acceptConnection() {

    sockaddr_in addr = {};
    socklen_t addrLen = sizeof(addr);

    int socket = accept(acceptingSocket, (sockaddr*) &addr, &addrLen);

    // if the connection is valid
    if (socket >= 0) {

        stringstream ss;
        ss << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);
        string name(ss.str());

        auto connection = new Connection(name, socket, *this);
        connections.insert({name, connection});

        parent.log((stringstream() << "New connection established " << *connection).str());
        connection->startReceiving();
    }
}

/**
 * Closes active connections with clients and the accepting socket
 */
void ReplicaConnectionHandler::close() {

    if (!openBool)
        return;

    openBool = false;

    #if defined(_WIN32) || defined(_WIN64)
        shutdown(acceptingSocket, SD_BOTH);
            closesocket(acceptingSocket);
            WSACleanup();
    #else
        shutdown(acceptingSocket, SHUT_RDWR);
        ::close(acceptingSocket);
    #endif

    acceptingThread->join();

    auto itr = connections.begin();
    while(itr != connections.cend()) {

        auto client = itr->second;
        client->close();
        itr = connections.erase(itr);
    }

    delete acceptingThread;
}

bool ReplicaConnectionHandler::isOpen() const {

    return openBool;
}

optional<ReplicaConnectionHandler::Connection*> ReplicaConnectionHandler::connectTo(const string &addressString, string *error) {

    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if(socket < 0) {

        if(error != nullptr) *error = "Couldn't create socket";
        return nullopt;
    }

    auto hostString = addressString.substr(0, addressString.find(':'));
    auto portString = addressString.substr(addressString.find(':') + 1, addressString.size());

    auto host = gethostbyname(hostString.c_str());
    if(host == nullptr) {

        if(error != nullptr) *error = "Couldn't find the selected host";
        return nullopt;
    }

    // Sets the address to connect to
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    memmove((char*) &addr.sin_addr.s_addr, (char*) host->h_addr, host->h_length);
    addr.sin_port = htons(stoi(portString));

    bool validConnection = connect(socket, (struct sockaddr *) &addr, sizeof(addr)) >= 0;
    if(!validConnection) {

        if(error != nullptr)
            *error = (stringstream() << "Couldn't connect to (address: " << addressString << ")").str();
        return nullopt;
    }

    auto connection = new Connection(addressString, socket, *this);
    connections.insert({addressString, connection});
    return make_optional(connection);
}

void ReplicaConnectionHandler::send(int socket, const Message &msg) const {

    if (!openBool)
        return;

    string msgString = msg.toString() + "\e\n";
    ::send(socket, msgString.c_str(), msgString.length(), 0);
}

Replica &ReplicaConnectionHandler::getParent() {

    return parent;
}
