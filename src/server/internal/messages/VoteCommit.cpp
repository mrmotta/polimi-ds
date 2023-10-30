//
// Created by Pierluigi Negro on 3/14/23.
//

#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::VoteCommit::VoteCommit(int_fast64_t uid, string key, int_fast64_t timestamp, Replica &replica) : ServerMessage(VOTE_COMMIT, replica) {
    this->uid = uid;
    this->key = std::move(key);
    this->timestamp = timestamp;
}

shared_ptr<Message> ServerMessage::VoteCommit::toMessage(const string& messageString, Replica &replica) {

    smatch sm;
    if (regex_match(messageString, sm, Regex::voteCommit))
        return make_shared<VoteCommit>(stoi(sm[1]), sm[2], stoi(sm[3]), replica);

    throw RegexMismatchException("Mismatch reading message VoteCommit");
}

string ServerMessage::VoteCommit::toString() const {
    return "VOTE_COMMIT; tId:" + std::to_string(uid) + ", k:\"" + key + "\", ts:" + std::to_string(timestamp);
}

void ServerMessage::VoteCommit::manage() const {
    replica.getCoordinator(key)->voteCommit(timestamp);
}
