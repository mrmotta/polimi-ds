//
// Created by Niccolò Nicolosi on 25/03/23.
//

#include <random>
#include "Coordinator.h"
#include "ServerMessage.h"

using namespace std;

Coordinator::Coordinator(ReplicaConnectionHandler &parent, ReplicaConnectionHandler::Connection *connection)
: parentHandler(parent), parentReplica(parent.getParent()), replies(0), uid(parentReplica.getNewReplicaTS()), values(), version(0) {
    this->connection = connection;
}

void Coordinator::get(const string &key) {

    pair<string, int_fast64_t> result;
    mt19937 gen(random_device {}());
    uniform_int_distribution<> distribution(100, 1000);

    while (!parentReplica.getDatastore().get(key, result))
        this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));

    if (result.second != 0)
        values.emplace_back(result);

    replies = 1;

    vector<ReplicaConnectionHandler::Connection *> quorum = connectToQuorum(parentHandler.readQuorum);

    if (quorum.empty())
        throw runtime_error("Couldn't connect to quorum");

    sendToReplicas(ServerMessage::Fetch(key, parentReplica), quorum);

    if (replies != parentHandler.readQuorum) {
        closeConnections(quorum);
        throw runtime_error("Some replica didn't reply");
    }

    if (values.empty()) {
        connection->send(Message::Value());
        closeConnections(quorum);
        return;
    }

    pair<string, int_fast64_t> &latest = getLatestValue();
    connection->send(Message::Value(latest.first));
    parentReplica.getDatastore().tryUpdate(key, latest.first, latest.second);
    sendToReplicas(ServerMessage::Repair(key, latest.first, latest.second, parentReplica), quorum, false);
}

/**
 * Creates a thread with coordinator behaviour, then waits for it to finish (blocking).
 * Thread behaviour:
 *  - calls prepare on datastore
 *  - defines quorum - 1 amount of participants to write to and sends prepare messages
 *  - sleeps until all participants responded (needs to be woken up)
 *  - sends global commit/abort message to replicas and sleeps until all responses are received
 *  - sends ack message back to client
 *
 * @param key the key of the transaction
 * @param value the value to insert
 * @throw runtime_error if datastore.prepare/commit/abort return false, or if quorum is not reached
 */
void Coordinator::put(const std::string &key, const std::string &value) {

    abort = false;

    if (!parentReplica.getDatastore().prepare(key, version, uid))
        throw runtime_error("Couldn't obtain Datastore lock");
    replies = 1;

    vector<ReplicaConnectionHandler::Connection *> quorum = connectToQuorum(parentHandler.writeQuorum);

    if (quorum.empty()) {
        parentReplica.getDatastore().abort(key, uid);
        throw runtime_error("Couldn't connect to quorum");
    }

    sendToReplicas(ServerMessage::Prepare(uid, key, value, parentReplica), quorum);

    if (replies != parentHandler.writeQuorum) {
        sendToReplicas(ServerMessage::GlobalAbort(uid, key, parentReplica), quorum, false);
        closeConnections(quorum);
        parentReplica.getDatastore().abort(key, uid);
        throw runtime_error("Some replica didn't reply");
    }

    if (abort) {
        sendToReplicas(ServerMessage::GlobalAbort(uid, key, parentReplica), quorum, false);
        closeConnections(quorum);
        parentReplica.getDatastore().abort(key, uid);
        throw runtime_error("Received vote abort");
    }

    version ++;
    parentReplica.getDatastore().commit(key, value, version);
    sendToReplicas(ServerMessage::GlobalCommit(uid, key, value, (int) version, parentReplica), quorum, false);

    connection->send(Message::Ack("Value \"" + value + "\" effectively inserted for key \"" + key + "\""));
    closeConnections(quorum);
}

void Coordinator::sync() {
    vector<ReplicaConnectionHandler::Connection *> quorum = connectToQuorum(parentHandler.replicaAddresses.size(), false);
    sendToReplicas(ServerMessage::Sync(parentReplica.getDatastore().getStatus(), parentReplica), quorum);
    closeConnections(quorum);
}

void Coordinator::voteCommit(int_fast64_t version) {
    scoped_lock lock{repliesLock};
    this->version = (version > this->version) ? version : this->version;
}

void Coordinator::voteAbort() {
    scoped_lock lock{repliesLock};
    abort = true;
}

pair<string, int_fast64_t> &Coordinator::getLatestValue() {

    int_fast64_t max = 0;

    for (auto &entry: values)
        if (entry.second > max)
            max = entry.second;

    for (auto &entry: values)
        if (entry.second == max)
            return entry;

    return values.front();
}

void Coordinator::addValue(const string &value, int_fast64_t version) {
    scoped_lock lock{repliesLock};
    values.emplace_back(make_pair(value, version));
}

vector<ReplicaConnectionHandler::Connection *> Coordinator::connectToQuorum(int quorum, bool safe) {

    vector<ReplicaConnectionHandler::Connection *> connections;
    vector<string> addresses(parentHandler.replicaAddresses);
    auto rng = default_random_engine{random_device{}()};
    std::shuffle(addresses.begin(), addresses.end(), rng);

    while (connections.size() < size_t(quorum - 1) && !addresses.empty()) {
        auto connection = parentHandler.connectTo(addresses.back());
        if (connection.has_value())
            connections.push_back(connection.value());
        addresses.pop_back();
    }

    if (safe && connections.size() != size_t(quorum - 1)) {
        for (auto connection: connections)
            connection->close();
        connections.clear();
    }

    return connections;
}

void Coordinator::sendToReplicas(const Message &message, const vector<ReplicaConnectionHandler::Connection *> &replicas, bool waitReplies) {

    vector<::thread *> threads;

    for (auto replica: replicas)
        threads.emplace_back(new ::thread([this, replica, &message, waitReplies]() -> void {
            replica->send(message);
            if (waitReplies)
                try {
                    replica->receive();
                    replies ++;
                } catch (...) {}
        }));

    for (auto thread: threads) {
        thread->join();
        delete thread;
    }
    threads.clear();
}

void Coordinator::closeConnections(const std::vector<ReplicaConnectionHandler::Connection *> &replicas) {

    for (auto replica: replicas)
        replica->close();
}
