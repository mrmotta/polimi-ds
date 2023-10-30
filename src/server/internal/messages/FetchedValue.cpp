//
// Created by Pierluigi Negro on 3/14/23.
//

#include <iostream>
#include <sstream>
#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::FetchedValue::FetchedValue(string key, Replica &replica) : ServerMessage(FETCHED_VALUE, replica) {
    this->key = std::move(key);
    this->valid = false;
}

ServerMessage::FetchedValue::FetchedValue(string key, string value, int_fast64_t timestamp, Replica &replica) : ServerMessage(FETCHED_VALUE, replica) {
    this->key = std::move(key);
    this->value = std::move(value);
    this->timestamp = timestamp;
    this->valid = true;
}

shared_ptr<Message> ServerMessage::FetchedValue::toMessage(const string& messageString, Replica &replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::fetchedValue))
        return make_shared<FetchedValue>(sm[1], sm[2], stoi(sm[3]), replica);
    if (regex_match(messageString, sm, Regex::fetchedValueNull))
        return make_shared<FetchedValue>(sm[1], replica);

    throw RegexMismatchException("Mismatch reading message FetchedValue");
}

string ServerMessage::FetchedValue::toString() const {
    return "FETCHED_VALUE; k:\"" + key + "\", v:" + (
        valid ? '\"' + value + "\", ts:" + std::to_string(timestamp) : "null"
    );
}

void ServerMessage::FetchedValue::manage() const {
    if (valid)
        replica.getCoordinator(key)->addValue(value, timestamp);
}
