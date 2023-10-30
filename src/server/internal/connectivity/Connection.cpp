//
// Created by Niccolò Nicolosi on 25/03/23.
//

#if defined(_WIN32) || defined(_WIN64)
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
#endif
#include <unistd.h>
#include <cstring>
#include "../ServerMessage.h"
#include "ReplicaConnectionHandler.h"

using namespace std;

ReplicaConnectionHandler::Connection::Connection(string name, const int socket, ReplicaConnectionHandler &parent)
        : socket(socket), name(std::move(name)), parentHandler(parent), parentReplica(parent.getParent()),
          connectionOpen(true), messageBuffer(), receivingThread() {}

ReplicaConnectionHandler::Connection::~Connection() {

    if(connectionOpen)
        close();
    receivingThread->join();
    delete receivingThread;
}

/**
 * Receives and manages a single message from the client.
 * Blocking method
 */
void ReplicaConnectionHandler::Connection::receive() {

    if (!connectionOpen)
        return;

    do {

        //Actually waits for a message iff there are no other messages in buffer
        //else it just consumes the next message in buffer
        if (bufferContent.length() == 0) {

            memset(messageBuffer, 0, sizeof(messageBuffer));

            if (recv(socket, messageBuffer, bufferSize, 0) <= 0)
                throw runtime_error((stringstream() << "Couldn't receive message from " << *this).str());

            bufferContent += messageBuffer;
        }

        int lineLen = (int) bufferContent.find('\n');
        if (lineLen == -1)
            lineLen = (int) bufferContent.length();
        string line = bufferContent.substr(0, lineLen);

        bufferContent = (bufferContent.length() - lineLen) != 0
                        ? bufferContent.substr(line.length() + 1, bufferContent.length())
                        : "";

        if (!line.ends_with('\e'))
            partialMsg = line;
        else {

            string msgString = partialMsg + line;
            partialMsg = "";

            auto msg = ServerMessage::toMessage(msgString, parentReplica);
            parentReplica.log((stringstream() << "Received: \e[33m(" << msgString.substr(0, msgString.size() - 1) << ")\e[0m").str());

            switch (msg->getType()) {
                case Message::FETCH:
                    dynamic_pointer_cast<ServerMessage::Fetch>(msg)->setConnection(this);
                    break;
                case Message::GET:
                    dynamic_pointer_cast<ServerMessage::Get>(msg)->setConnection(this);
                    break;
                case Message::PREPARE:
                    dynamic_pointer_cast<ServerMessage::Prepare>(msg)->setConnection(this);
                    break;
                case Message::PUT:
                    dynamic_pointer_cast<ServerMessage::Put>(msg)->setConnection(this);
                    break;
                case Message::REPAIR:
                    dynamic_pointer_cast<ServerMessage::Repair>(msg)->setConnection(this);
                    break;
                case Message::SYNC:
                    dynamic_pointer_cast<ServerMessage::Sync>(msg)->setConnection(this);
                    break;
                default:;
            }
            msg->manage();
        }

    } while(partialMsg.length() != 0);
}

void ReplicaConnectionHandler::Connection::send(const Message &msg) const {

    if (!connectionOpen)
        return;

    parentHandler.send(socket, msg);
    parentReplica.log((stringstream() << "Sent: \e[33m(" << msg.toString() << ")\e[0m to client " << *this).str());
}

/**
 * Starts receiving messages from the client on a separate thread
 */
void ReplicaConnectionHandler::Connection::startReceiving() {

    receivingThread = new thread([this]() -> void {

        while(connectionOpen)
            try {
                receive();
            } catch(runtime_error &error) {
                parentReplica.log(error.what(), true);
                close();
            }
    });
}

/**
 * Closes the connection with the client
 */
void ReplicaConnectionHandler::Connection::close() {

    if (!connectionOpen)
        return;

    connectionOpen = false;

    #if defined(_WIN32) || defined(_WIN64)
        shutdown(socket, SD_BOTH);
        closesocket(socket);
    #else
        shutdown(socket, SHUT_RDWR);
        ::close(socket);
    #endif

    parentReplica.log((stringstream() << "Closed connection with client " << *this).str());
}

bool ReplicaConnectionHandler::Connection::isOpen() const {

    return connectionOpen;
}
