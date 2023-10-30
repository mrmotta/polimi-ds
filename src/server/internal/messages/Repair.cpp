//
// Created by Pierluigi Negro on 3/15/23.
//

#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::Repair::Repair(string key, string value, int_fast64_t timestamp, Replica &replica) : ServerMessage(REPAIR, replica) {
    this->key = std::move(key);
    this->value = std::move(value);
    this->timestamp = timestamp;
}

shared_ptr<Message> ServerMessage::Repair::toMessage(const string& messageString, Replica &replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::repair))
        return make_shared<Repair>(sm[1], sm[2], stoi(sm[3]), replica);

    throw RegexMismatchException("Mismatch reading message Repair");
}

string ServerMessage::Repair::toString() const {
    return "REPAIR; k:\"" + key + "\", v:\"" + value + "\", ts:" + std::to_string(timestamp) ;
}

void ServerMessage::Repair::manage() const {
    replica.getDatastore().tryUpdate(key, value, timestamp);
    connection->close();
}

void ServerMessage::Repair::setConnection(ReplicaConnectionHandler::Connection *connection) {
    this->connection = connection;
}
