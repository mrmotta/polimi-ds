//
// Created by Pierluigi Negro on 3/14/23.
//

#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::GlobalCommit::GlobalCommit(int_fast64_t uid, string key, string value, int timestamp, Replica &replica) : ServerMessage(GLOBAL_COMMIT, replica) {
    this->uid = uid;
    this->key = std::move(key);
    this->value = std::move(value);
    this->timestamp = timestamp;
}

shared_ptr<Message> ServerMessage::GlobalCommit::toMessage(const string& messageString, Replica &replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::globalCommit))
        return make_shared<GlobalCommit>(stoi(sm[1]), sm[2], sm[3], stoi(sm[4]), replica);

    throw RegexMismatchException("Mismatch reading message GlobalCommit");
}

string ServerMessage::GlobalCommit::toString() const {
    return "GLOBAL_COMMIT; tId:" + std::to_string(uid) + ", k:\"" + key + "\", v:\"" + value + "\", ts:" + std::to_string(timestamp) ;
}

void ServerMessage::GlobalCommit::manage() const {
    replica.getParticipant(key)->globalCommit(timestamp);
}
