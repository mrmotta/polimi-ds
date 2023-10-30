//
// Created by Pierluigi Negro on 3/15/23.
//

#include <random>
#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::Sync::Sync(const unordered_map<string, int_fast64_t> &entries, Replica &replica) : ServerMessage(SYNC, replica) {
    this->entries = entries;
}

shared_ptr<Message> ServerMessage::Sync::toMessage(const string &messageString, Replica &replica) {

    smatch sm;
    unordered_map<string, int_fast64_t> entries;
    if (!regex_match(messageString, sm, Regex::syncStart))
        throw RegexMismatchException("Mismatch reading message Sync");

    string remaining = sm[1];
    if (remaining.starts_with("none"))
        return make_shared<Sync>(entries, replica);

    while (regex_match(remaining, sm, Regex::sync)) {
        entries.emplace(sm[1], stoll(sm[2]));
        remaining = sm[3];
    }

    if (!regex_match(remaining, sm, Regex::syncEnd))
        throw RegexMismatchException("Mismatch reading message Sync");

    entries.emplace(sm[1], stoll(sm[2]));

    return make_shared<Sync>(entries, replica);
}

string ServerMessage::Sync::toString() const {

    if (entries.empty())
        return "SYNC; none";

    string res = "SYNC; ";
    for (auto &entry: entries)
        res.append("(k:\"" + entry.first + "\", ts:" + to_string(entry.second) + "), ");
    res.pop_back();
    res.pop_back();

    return res;
}

void ServerMessage::Sync::manage() const {

    unordered_map<string, int_fast64_t> status = replica.getDatastore().getStatus();
    unordered_map<string, pair<string, int_fast64_t>> result;
    pair<string, int_fast64_t> value;
    mt19937 gen(random_device {}());
    uniform_int_distribution<> distribution(100, 1000);

    for (auto &statusEntry: status) {
        auto entry = entries.find(statusEntry.first);
        if (entry == entries.end() || entry->second < statusEntry.second) {
            while (!replica.getDatastore().get(statusEntry.first, value))
                this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));
            result.emplace(statusEntry.first, value);
        }
    }

    connection->send(ServerMessage::Update(result, replica));
}

void ServerMessage::Sync::setConnection(ReplicaConnectionHandler::Connection *connection) {
    this->connection = connection;
}
