//
// Created by Pierluigi Negro on 3/12/23.
//

#include <utility>
#include <random>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::Put::Put(string key, string value, Replica &replica) : ServerMessage(PUT, replica) {
    this->key = std::move(key);
    this->value = std::move(value);
}

/**
 * Creates coordinator for the key, makes sure that coordinator.put() is carried out
 * To respect lock hierarchy, in a loop:
 *  - coordinator is added to replica, otherwise: wait random time -> try again
 *  - coordinator.put() is called, if error: remove coordinator from replica, wait random time, try again
 *  - remove coordinator from replica, break
 */
void ServerMessage::Put::manage() const {

    shared_ptr<Coordinator> coordinator = make_shared<Coordinator>(replica.getReplicaConnectionHandler(), connection);
    mt19937 gen(random_device {}());
    uniform_int_distribution<> distribution(100, 1000);
    bool done = false;

    replica.acceptRequest();

    do {
        while (!replica.addCoordinator(key, coordinator))
            this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));
        try {
            coordinator->put(key, value);
            replica.removeCoordinator(key);
            done = true;
        } catch (...) {
            replica.removeCoordinator(key);
            this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));
        }
    } while (!done);
}

void ServerMessage::Put::setConnection(ReplicaConnectionHandler::Connection *connection) {
    this->connection = connection;
}

shared_ptr<Message> ServerMessage::Put::toMessage(const string& messageString, Replica &replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::put))
        return make_shared<Put>(sm[1], sm[2], replica);

    throw RegexMismatchException("Mismatch reading message Put");
}

string ServerMessage::Put::toString() const {
    return "PUT; k:\"" + key + "\", v:\"" + value + "\"";
}
