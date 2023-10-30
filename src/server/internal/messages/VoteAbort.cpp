//
// Created by Pierluigi Negro on 3/14/23.
//

#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::VoteAbort::VoteAbort(int_fast64_t uid, string key, Replica &replica) : ServerMessage(VOTE_ABORT, replica) {
    this->uid = uid;
    this->key = std::move(key);
}

shared_ptr<Message> ServerMessage::VoteAbort::toMessage(const string& messageString, Replica &replica) {

    smatch sm;
    if (regex_match(messageString, sm, Regex::voteAbort))
        return make_shared<VoteAbort>(stoi(sm[1]), sm[2], replica);

    throw RegexMismatchException("Mismatch reading message VoteAbort");
}

string ServerMessage::VoteAbort::toString() const {
    return "VOTE_ABORT; tId:" + std::to_string(uid) + ", k:\"" + key +"\"";
}

void ServerMessage::VoteAbort::manage() const {
    replica.getCoordinator(key)->voteAbort();
}
