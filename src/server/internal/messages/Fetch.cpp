//
// Created by Pierluigi Negro on 3/14/23.
//

#include <random>
#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::Fetch::Fetch(string key, Replica &replica) : ServerMessage(FETCH, replica) {
    this->key = std::move(key);
}

shared_ptr<Message> ServerMessage::Fetch::toMessage(const string& messageString, Replica &replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::fetch))
        return make_shared<Fetch>(sm[1], replica);

    throw RegexMismatchException("Mismatch reading message Fetch");
}

string ServerMessage::Fetch::toString() const {
    return "FETCH; k:\"" + key + "\"";
}

void ServerMessage::Fetch::manage() const {

    pair<string, int_fast64_t> result;
    mt19937 gen(random_device {}());
    uniform_int_distribution<> distribution(100, 1000);

    replica.acceptRequest();

    while (!replica.getDatastore().get(key, result))
        this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));

    if (result.second != 0)
        connection->send(ServerMessage::FetchedValue(key, result.first, result.second, replica));
    else
        connection->send(ServerMessage::FetchedValue(key, replica));
}

void ServerMessage::Fetch::setConnection(ReplicaConnectionHandler::Connection *connection) {
    this->connection = connection;
}
