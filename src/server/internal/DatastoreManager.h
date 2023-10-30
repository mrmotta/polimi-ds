#ifndef LEADERLESS_DATASTORE_DATASTOREMANAGER_H
#define LEADERLESS_DATASTORE_DATASTOREMANAGER_H

#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>

class DatastoreManager {

    public:
    DatastoreManager() {}

    public:
    DatastoreManager(DatastoreManager const&) = delete;
    void operator=(DatastoreManager const&) = delete;

    private:
    std::string path;
    std::atomic_bool pending = false;
    std::atomic_bool stop = false;

    private:
    std::unordered_map<std::string, std::pair<std::string, int_fast64_t>> datastore;
    std::unordered_map<std::string, int_fast64_t> owner2pc;

    private:
    std::shared_mutex datastoreMutex;
    std::mutex pendingMutex;

    private:
    std::thread writeThread;

    public:
    bool get(std::string const&key, std::pair<std::string, int_fast64_t> &result);
    std::unordered_map<std::string, int_fast64_t> getStatus();
    void initialize(std::string const&path, const std::chrono::duration<int, std::milli> writeCheckDelay);
    void load();
    void tryUpdate(std::string const&key, std::string const&value, int_fast64_t version);

    public:
    bool prepare(std::string const&key, int_fast64_t &version, int_fast64_t uid);
    void commit(std::string const&key, std::string const&value, int_fast64_t version);
    void abort(std::string const&key, int_fast64_t uid);

    public:
    void close();

    private:
    void dump(const std::chrono::duration<int, std::milli> writeCheckDelay);
};

#endif
