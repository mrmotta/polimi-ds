//
// Created by Pierluigi Negro on 3/14/23.
//

#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::Update::Update(const unordered_map<string, pair<string, int_fast64_t>> &newValues, Replica &replica) : ServerMessage(UPDATE, replica) {
    this->newValues = newValues;
}

shared_ptr<Message> ServerMessage::Update::toMessage(const string& messageString, Replica &replica) {

    smatch sm;
    unordered_map<string, pair<string, int_fast64_t>> values;
    if (!regex_match(messageString, sm, Regex::updateStart))
        throw RegexMismatchException("Mismatch reading message Update");

    string remaining = sm[1];
    if (remaining.starts_with("none"))
        return make_shared<Update>(values, replica);

    while (regex_match(remaining, sm, Regex::update)) {
        values.emplace(sm[1], make_pair(sm[2], stoll(sm[3])));
        remaining = sm[4];
    }

    if (!regex_match(remaining, sm, Regex::updateEnd))
        throw RegexMismatchException("Mismatch reading message Update");

    values.emplace(sm[1], make_pair(sm[2], stoll(sm[3])));

    return make_shared<Update>(values, replica);
}

string ServerMessage::Update::toString() const {

    if (newValues.empty())
        return "UPDATE; none";

    string res = "UPDATE; ";
    for (auto &entry: newValues)
        res.append("(k:\"" + entry.first + "\", v:\"" + entry.second.first + "\", ts:" + to_string(entry.second.second) + "), ");
    res.pop_back();
    res.pop_back();

    return res;
}

void ServerMessage::Update::manage() const {

    for (auto &entry: newValues)
        replica.getDatastore().tryUpdate(entry.first, entry.second.first, entry.second.second);    
}
