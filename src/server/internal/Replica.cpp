//
// Created by Niccolò Nicolosi on 06/03/23.
//

#include <iostream>
#include <random>
#include "Coordinator.h"
#include "Logger.h"
#include "Replica.h"

#ifndef DEBUG_DEFAULT
#define DEBUG_DEFAULT true
#endif

using namespace std;

Replica::Replica(ServerConfig &config)
: config(config), alive(false), debug(DEBUG_DEFAULT), synchronizing(true), address({}),
connectionHandler(*this, config.getReplicas(),
                  config.getReadQuorum(), config.getWriteQuorum()) {

    //sets the address family to IPv4
    address.sin_family = AF_INET;

    //takes the host address automatically
    address.sin_addr.s_addr = INADDR_ANY;

    //sets the port number, converting an int to TCP/IP network byte order (big endian)
    address.sin_port = htons(config.getPort());

    datastore.initialize(config.getDatastorePath(), 5s);

    if (config.loadDatastoreFromFile())
        datastore.load();

    replicaTS = config.getReplicaId();
}

Replica::~Replica() {

    if(alive)
        shutdown();
}

/**
 * Starts the replica.
 * Blocking method if async is set to false (default)
 * @param async true if the replica has to run asynchronously
 */
void Replica::start(bool async) {

    std::cout << "Starting the replica at port " << config.getPort() << "\n";
    alive = true;
    connectionHandler.open(async);

    Coordinator(connectionHandler, nullptr).sync();
    synchronized();
}

void Replica::shutdown() {

    if (alive) {

        log("Shutting down");
        alive = false;
        if (connectionHandler.isOpen())
            connectionHandler.close();
        datastore.close();
        log("Stopped datastore");
    }
}

bool Replica::isAlive() const {

    return alive;
}

bool Replica::isDebug() const {

    return debug;
}

void Replica::setDebug(bool debug) {

    this->debug = debug;
}

void Replica::log(const std::string &msg, bool error) const {
    if(debug)
        cout << (stringstream() << (error ? "\e[31m" : "\e[32m")
             << "[Replica (port: " << config.getPort() << ")]\e[0m: " << msg << "\n").str();
}

void Replica::log(const std::stringstream &msg, bool error) const {
    if(debug)
        cout << (stringstream() << (error ? "\e[31m" : "\e[32m")
             << "[Replica (port: " << config.getPort() << ")]\e[0m: " << msg.str() << "\n").str();
}

const sockaddr_in &Replica::getAddress() const {

    return address;
}

DatastoreManager &Replica::getDatastore() {

    return datastore;
}

int_fast64_t Replica::getNewReplicaTS() {
    replicaTSLock.lock();
    int_fast64_t result = replicaTS;
    replicaTS += config.getReplicasAmount();
    replicaTSLock.unlock();
    return result;
}

ReplicaConnectionHandler &Replica::getReplicaConnectionHandler() {
    return connectionHandler;
}

shared_ptr<Coordinator> Replica::getCoordinator(const std::string &key) {
    scoped_lock lock(coorParLock);
    return coordinators.at(key);
}

bool Replica::addCoordinator(const std::string &key, shared_ptr<Coordinator> coordinator) {
    scoped_lock lock(coorParLock);
    if (coordinators.contains(key) || participants.contains(key))
        return false;
    coordinators.emplace(key, coordinator);
    return true;
}

void Replica::removeCoordinator(const std::string &key) {
    scoped_lock lock(coorParLock);
    coordinators.erase(key);
}

shared_ptr<Participant> Replica::getParticipant(const std::string &key) {
    scoped_lock lock(coorParLock);
    return participants.at(key);
}

bool Replica::addParticipant(const std::string &key, shared_ptr<Participant> participant) {
    scoped_lock lock(coorParLock);
    if(coordinators.contains(key) || participants.contains(key))
        return false;
    participants.emplace(key, participant);
    return true;
}

void Replica::removeParticipant(const std::string &key) {
    scoped_lock lock(coorParLock);
    participants.erase(key);
}

void Replica::synchronized() {
    synchronizing = false;
}

void Replica::acceptRequest() {
    mt19937 gen(random_device {}());
    uniform_int_distribution<> distribution(100, 1000);
    while (synchronizing)
        this_thread::sleep_for(chrono::duration<int, milli>(distribution(gen)));
}
