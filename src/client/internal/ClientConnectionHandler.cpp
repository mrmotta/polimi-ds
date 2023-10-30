//
// Created by Niccolò Nicolosi on 06/03/23.
//

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <cstring>
    #include <netinet/in.h>
#endif
#include <stdexcept>
#include <iostream>
#include <random>
#include <utility>
#include "ClientMessage.h"
#include "ClientConnectionHandler.h"

using namespace std;

ClientConnectionHandler::ClientConnectionHandler(vector<string> replicaAddresses)
: currReplicaSocket(), messageBuffer(), replicaAddresses(std::move(replicaAddresses))
#if defined(_WIN32) || defined(_WIN64)
, wsa({})
#endif
{

    #if defined(_WIN32) || defined(_WIN64)
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
            throw runtime_error((stringstream() << "Could not initialize Windows' socket subsystem. Error Code: " << WSAGetLastError()).str());
    #endif
}

ClientConnectionHandler::~ClientConnectionHandler() {

    disconnect();

    #if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
    #endif
}

/**
 * Connects to a random replica of the datastore
 **/
bool ClientConnectionHandler::connectToReplica() {

    bool validConnection;
    hostent *replicaHost;
    sockaddr_in replicaAddr = {};

    // Sets the address family to IPv4
    replicaAddr.sin_family = AF_INET;

    // Sets the random engine (needed to shuffle)
    auto rng = default_random_engine {random_device {}()};

    vector<string> replicaAddressesCopy(replicaAddresses);
    shuffle(replicaAddressesCopy.begin(), replicaAddressesCopy.end(), rng);

    string replicaName, addrString, portString;
    do {

        try {

            currReplicaSocket = socket(AF_INET, SOCK_STREAM, 0);
            if(currReplicaSocket < 0)
                throw runtime_error("Couldn't create socket");

            replicaName = replicaAddressesCopy.back();
            replicaAddressesCopy.pop_back();

            addrString = replicaName.substr(0, replicaName.find(':'));
            portString = replicaName.substr(replicaName.find(':') + 1, replicaName.size());

            replicaHost = gethostbyname(addrString.c_str());
            if(replicaHost == nullptr)
                throw runtime_error("Couldn't find the selected host");

            // Sets the replica address and port to connect to
            memmove((char*) &replicaAddr.sin_addr.s_addr, (char*) replicaHost->h_addr, replicaHost->h_length);
            replicaAddr.sin_port = htons(stoi(portString));

            validConnection = connect(currReplicaSocket, (struct sockaddr *) &replicaAddr, sizeof(replicaAddr)) >= 0;
            if(!validConnection)
                throw runtime_error((stringstream() << "Couldn't connect to the selected replica (address: "
                                                    << replicaName << ")").str());
        } catch(runtime_error& error) {

            #if defined(_WIN32) || defined(_WIN64)
                shutdown(currReplicaSocket, SD_BOTH);
                closesocket(currReplicaSocket);
            #else
                shutdown(currReplicaSocket, SHUT_RDWR);
                ::close(currReplicaSocket);
            #endif

            validConnection = false;
        }
    } while (!validConnection && !replicaAddressesCopy.empty());

    if (!validConnection) {
        cout << "It wasn't possible to establish a connection with the server.\nPlease, try again later.\n";
        return false;
    }
    return true;
}

/**
 * Disconnects from the current replica
 */
void ClientConnectionHandler::disconnect() const {

    // cout << "Disconnecting from the current replica\n";
    #if defined(_WIN32) || defined(_WIN64)
        shutdown(currReplicaSocket, SD_BOTH);
        closesocket(currReplicaSocket);
    #else
        shutdown(currReplicaSocket, SHUT_RDWR);
        ::close(currReplicaSocket);
    #endif
}

void ClientConnectionHandler::send(const Message &msg) const {

    string msgString = msg.toString() + "\e\n";
    ::send(currReplicaSocket, msgString.c_str(), msgString.length(), 0);
}

/**
 * Receives a single message from the current replica.
 * Blocking method
 */
void ClientConnectionHandler::receive() {

    do {

        //Actually waits for a message iff there are no other messages in buffer
        //else it just consumes the next message in buffer
        if(bufferContent.length() == 0) {

            memset(messageBuffer, 0, sizeof(messageBuffer));

            if(::recv(currReplicaSocket, messageBuffer, bufferSize, 0) <= 0) {
                cout << "Something went wrong while waiting for a reply from the server.\nPlease, try again later.\n";
                disconnect();
                #if defined(_WIN32) || defined(_WIN64)
                    WSACleanup();
                #endif
                return;
            }

            bufferContent += messageBuffer;
        }

        int lineLen = (int) bufferContent.find('\n');
        if(lineLen == -1) lineLen = (int) bufferContent.length();
        string line = bufferContent.substr(0, lineLen);

        bufferContent = (bufferContent.length() - lineLen) != 0 ?
                        bufferContent.substr(line.length() + 1, bufferContent.length()) : "";

        if(!line.ends_with('\e')) partialMsg = line;
        else {

            string msgString = partialMsg + line;
            partialMsg = "";

            auto msg = Message::toMessage(msgString);
            msg.get()->manage();
        }

    } while(partialMsg.length() != 0);
}
