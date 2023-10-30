//
// Created by Pierluigi Negro on 3/24/23.
//

#include "Participant.h"
#include "ServerMessage.h"

using namespace std;

Participant::Participant(ReplicaConnectionHandler &parent, ReplicaConnectionHandler::Connection *connection, int_fast64_t uid)
: parentReplica(parent.getParent()) {
    this -> connection = connection;
    this -> uid = uid;
}

/**
 * Creates a thread with participant behaviour, than waits for it to finish (blocking)
 * thread behaviour:
 *  - calls prepare on datastore, if unable sends voteAbort to coordinator
 *  - sends voteCommit to coordinator with local timestamp
 *  - sleeps until answer is received (until timestamp is updated or abort is received)
 *  - commits or abort transaction on datastore, sends ack if so happens
 *  - sends nack if commit or abort cannot be called on datastore and throws runtime_error
 *
 * @param key   the key of the transaction
 * @param value the value to insert
 * @throw runtime_error if commit/abort cannot be conducted in datastore
 */
void Participant::prepare2PC(const std::string &key, const std::string &value, const int_fast64_t uid) {

    if (!parentReplica.getDatastore().prepare(key, timestamp, uid)) {
        connection->send(ServerMessage::VoteAbort(uid, key, parentReplica));
        return;
    }

    connection->send(ServerMessage::VoteCommit(uid, key, timestamp, parentReplica));
    connection->receive();

    if (abort) {
        parentReplica.getDatastore().abort(key, uid);
        connection->send(Message::Ack("Aborting transaction"));
        return;
    }

    parentReplica.getDatastore().commit(key, value, timestamp);
    connection->send(Message::Ack("Commit successful"));
}

void Participant::globalAbort() {
    scoped_lock lock(replyLock);
    abort = true;
}

void Participant::globalCommit(int_fast64_t timestamp) {
    scoped_lock lock(replyLock);
    this->timestamp = timestamp;
}

int_fast64_t Participant::getUid() {
    return uid;
}

ReplicaConnectionHandler::Connection *Participant::getConnection() {
    return connection;
}
