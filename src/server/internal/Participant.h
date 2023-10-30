//
// Created by Pierluigi Negro on 3/24/23.
//

#ifndef LEADERLESS_DATASTORE_PARTICIPANT_H
#define LEADERLESS_DATASTORE_PARTICIPANT_H

#include <thread>
#include <condition_variable>
#include "connectivity/ReplicaConnectionHandler.h"

class Participant {

private:
    Replica &parentReplica;
    ReplicaConnectionHandler::Connection *connection;
    std::string key, value;
    int_fast64_t timestamp;
    std::mutex replyLock;
    bool abort = false;
    int_fast64_t uid;

public:
    int_fast64_t getUid();
    ReplicaConnectionHandler::Connection *getConnection();
    explicit Participant(ReplicaConnectionHandler &parent, ReplicaConnectionHandler::Connection *connection, int_fast64_t uid);

    void prepare2PC(const std::string &key, const std::string &value, const int_fast64_t uid);
    void globalAbort();
    void globalCommit(int_fast64_t timestamp);
};

#endif //LEADERLESS_DATASTORE_PARTICIPANT_H
