//
// Created by Pierluigi Negro on 06/03/23.
//

#include <random>
#include <string>
#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::Get::Get(string key, Replica &replica) : ServerMessage(GET, replica) {
    this->key = std::move(key);
}

void ServerMessage::Get::manage() const {

    shared_ptr<Coordinator> coordinator = make_shared<Coordinator>(replica.getReplicaConnectionHandler(), connection);
    mt19937 gen(random_device {}());
    uniform_int_distribution<> distribution(100, 1000);
    bool done = false;

    replica.acceptRequest();

    do {
        while (!replica.addCoordinator(key, coordinator))
            this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));
        try {
            coordinator->get(key);
            replica.removeCoordinator(key);
            done = true;
        } catch (...) {
            replica.removeCoordinator(key);
            this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));
        }
    } while (!done);
}

void ServerMessage::Get::setConnection(ReplicaConnectionHandler::Connection *connection) {
    this->connection = connection;
}

shared_ptr<Message> ServerMessage::Get::toMessage(const string& messageString, Replica& replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::get))
        return make_shared<Get>(sm[1], replica);

    throw RegexMismatchException("Mismatch reading message Get");
}

string ServerMessage::Get::toString() const {
    return "GET; k:\"" + key + "\"";
}
