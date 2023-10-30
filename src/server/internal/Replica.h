//
// Created by Niccolò Nicolosi on 06/03/23.
//

#ifndef LEADERLESS_DATASTORE_REPLICA_H
#define LEADERLESS_DATASTORE_REPLICA_H


#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#else
    #include <netinet/tcp.h>
#endif
#include <atomic>
#include <mutex>
#include "Coordinator.h"
#include "DatastoreManager.h"
#include "Participant.h"
#include "ServerConfig.h"
#include "connectivity/ReplicaConnectionHandler.h"

class Replica {

    private:
    const ServerConfig &config;
    std::atomic_bool alive, debug, synchronizing;
    sockaddr_in address;
    ReplicaConnectionHandler connectionHandler;
    DatastoreManager datastore;
    int replicaTS;
    std::mutex replicaTSLock;
    std::unordered_map<std::string, std::shared_ptr<Coordinator>> coordinators;
    std::unordered_map<std::string, std::shared_ptr<Participant>> participants;
    std::mutex coorParLock;
    public:
    Replica(ServerConfig &config);
    ~Replica();

    void start(bool async = false);
    void shutdown();
    bool isAlive() const;
    bool isDebug() const;
    void setDebug(bool debug);
    void log(const std::string &msg, bool error = false) const;
    void log(const std::stringstream &msg, bool error = false) const;

    const sockaddr_in &getAddress() const;
    DatastoreManager &getDatastore();
    ReplicaConnectionHandler &getReplicaConnectionHandler();
    std::shared_ptr<Coordinator> getCoordinator(const std::string &key);
    int_fast64_t getNewReplicaTS();
    bool addCoordinator(const std::string &key, std::shared_ptr<Coordinator> coordinator);
    void removeCoordinator(const std::string &key);
    std::shared_ptr<Participant> getParticipant(const std::string &key);
    bool addParticipant(const std::string &key, std::shared_ptr<Participant> participant);
    void removeParticipant(const std::string &key);

    void synchronized();
    void acceptRequest();
};

#endif //LEADERLESS_DATASTORE_REPLICA_H
