//
// Created by Niccolò Nicolosi on 25/03/23.
//

#ifndef LEADERLESS_DATASTORE_COORDINATOR_H
#define LEADERLESS_DATASTORE_COORDINATOR_H

#include <atomic>
#include <mutex>
#include <thread>
#include "connectivity/ReplicaConnectionHandler.h"
#include "../../common/Message.h"

class Coordinator {

private:
    std::atomic_bool abort = false;
    ReplicaConnectionHandler::Connection *connection;
    ReplicaConnectionHandler &parentHandler;
    Replica &parentReplica;
    std::atomic_int replies;
    std::mutex repliesLock;
    const int_fast64_t uid;
    std::vector<std::pair<std::string, int_fast64_t>> values;
    int_fast64_t version;

public:
    explicit Coordinator(ReplicaConnectionHandler &parent, ReplicaConnectionHandler::Connection *connection);

    void get(const std::string &key);
    void put(const std::string &key, const std::string &value);
    void sync();

    void addValue(const std::string &value, int_fast64_t version);

    void voteCommit(int_fast64_t version);
    void voteAbort();

private:
    std::pair<std::string, int_fast64_t> &getLatestValue();
    std::vector<ReplicaConnectionHandler::Connection *> connectToQuorum(int quorum, bool safe = true);
    void sendToReplicas(const Message &message, const std::vector<ReplicaConnectionHandler::Connection *> &replicas, bool waitReplies = true);
    void closeConnections(const std::vector<ReplicaConnectionHandler::Connection *> &replicas);
};

#endif //LEADERLESS_DATASTORE_COORDINATOR_H
