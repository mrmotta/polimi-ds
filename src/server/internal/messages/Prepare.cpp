//
// Created by Pierluigi Negro on 3/14/23.
//

#include <utility>
#include "../ServerMessage.h"

using namespace std;

ServerMessage::Prepare::Prepare(int_fast64_t uid, string key, string value, Replica &replica) : ServerMessage(PREPARE, replica) {
    this->uid = uid;
    this->key = std::move(key);
    this->value = std::move(value);
}

shared_ptr<Message> ServerMessage::Prepare::toMessage(const string& messageString, Replica &replica) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::prepare))
        return make_shared<Prepare>(stoi(sm[1]), sm[2], sm[3], replica);

    throw RegexMismatchException("Mismatch reading message Prepare");
}

string ServerMessage::Prepare::toString() const {
    return "PREPARE; tId:" + std::to_string(uid) + ", k:\"" + key + "\", v:\"" + value + "\"";
}

/**
 * Creates participant for the key, tries to insert into replica, if
 * unable to do so: sends voteAbort back to coordinator.
 */
void ServerMessage::Prepare::manage() const {
    shared_ptr<Participant> participant = make_shared<Participant>(replica.getReplicaConnectionHandler(), connection, uid);
    replica.acceptRequest();
    if (!replica.addParticipant(key, participant)) {
        connection->send(VoteAbort(uid, key, replica));
        return;
    }
    participant->prepare2PC(key, value, uid);
    replica.removeParticipant(key);
}

void ServerMessage::Prepare::setConnection(ReplicaConnectionHandler::Connection *connection) {
    this->connection = connection;
}
