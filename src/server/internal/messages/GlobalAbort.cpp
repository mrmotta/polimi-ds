//
// Created by Pierluigi Negro on 3/14/23.
//

#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::GlobalAbort::GlobalAbort(int_fast64_t uid, string key, Replica &replica) : ServerMessage(GLOBAL_ABORT, replica) {
    this->uid = uid;
    this->key = std::move(key);
}

shared_ptr<Message> ServerMessage::GlobalAbort::toMessage(const string& messageString, Replica &replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::globalAbort))
        return make_shared<GlobalAbort>(stoi(sm[1]), sm[2], replica);

    throw RegexMismatchException("Mismatch reading message GlobalAbort");
}

string ServerMessage::GlobalAbort::toString() const {
    return "GLOBAL_ABORT; tId:" + std::to_string(uid) + ", k:\"" + key + "\"";
}

void ServerMessage::GlobalAbort::manage() const {
    shared_ptr<Participant> par;
    try {
        par = replica.getParticipant(key);
        if (par->getUid() == uid)
            par->globalAbort();
    } catch (...) {}
}
