#include <regex>
#include "DatastoreManager.h"

using namespace std;

/**
 * Aborts the 2PC on the specified key.
 * It must called after the prepare function on the same key (if an abort state has been reached). Thing may go wrong otherwise.
 * 
 * @param key Reference to the key on which performing the operation.
 * @param uid ID of the transaction.
 */
void DatastoreManager::abort(string const&key, int_fast64_t uid) {

    unique_lock lock(datastoreMutex);

    if (owner2pc.at(key) == uid)
        owner2pc.erase(key);
}

/**
 * Closes the datastore, saving all data on disk.
 */
void DatastoreManager::close() {

    stop = true;
    writeThread.join();
}

/**
 * Commits the new value (with it's new version number) associated to a specific key to the datastore.
 * It must called after the prepare function on the same key (if a commit state has been reached). Thing may go wrong otherwise.
 * 
 * @param key Reference to the key on which performing the operation.
 * @param value Reference to the value to store.
 * @param version Version number of the stored value. It must be greater than the current value (if present), otherwise the operation is aborted.
 */
void DatastoreManager::commit(string const&key, string const&value, int_fast64_t version) {

    unordered_map<string, pair<string, int_fast64_t>>::iterator entry;

    unique_lock lock(datastoreMutex);

    entry = datastore.find(key);

    if (entry != datastore.end()) {

        pair<string, int_fast64_t> &location = entry->second;

        if (location.second < version) {
            location.first = value;
            location.second = version;
            pending = true;
        }
    } else {

        datastore.emplace(key, make_pair(value, version));
        pending = true;
    }
    owner2pc.erase(key);
}

/**
 * Dumps the whole datastore to the specified file (which must be present).
 * 
 * @param writeCheckDelay Time to wait before checking again if there is some operation to perform on disk.
 */
void DatastoreManager::dump(const chrono::duration<int, milli> writeCheckDelay) {

    ofstream file;
    unordered_map<string, pair<string, int_fast64_t>> localDatastore;
    bool localPending = false;
    bool localStop = false;

    while (true) {

        {
            scoped_lock pendingLock(pendingMutex);

            localStop = stop;
            localPending = pending;

            if (pending) {
                shared_lock datastoreLock(datastoreMutex);
                localDatastore = datastore;
                pending = false;
            }
        }

        if (localPending) {

            file.open(path);

            for (auto entry: localDatastore)
                file << '"'
                    << entry.first
                    << "\",\""
                    << entry.second.first
                    << "\","
                    << entry.second.second
                    << endl;

            file.close();
            localDatastore.clear();
        }

        if (localStop)
            break;

        this_thread::sleep_for(writeCheckDelay);
    }
}

/**
 * Allows to retrieve both the value and the version of the data associated to a specific key.
 * If a 2PC is running for that key, it's blocking.
 * 
 * @param key Reference to the key to search.
 * @param result Reference to a pair where to copy both the value and the version of the retrieved data. If the key is not present, the version number is set to 0.
 * 
 * @returns Whether the process completed successfully.
 */
bool DatastoreManager::get(string const&key, pair<string, int_fast64_t> &result) {

    unordered_map<string, pair<string, int_fast64_t>>::iterator entry;

    shared_lock lock(datastoreMutex);

    if (owner2pc.contains(key))
        return false;

    entry = datastore.find(key);

    if (entry != datastore.end()) {
        result.first = entry->second.first;
        result.second = entry->second.second;
    } else
        result.second = 0;

    return true;
}

/**
 * Returns all the stored keys together with their version.
 * If there is no stored value, or it hasn't been initialized, the returned map doesn't contain any element.
 * 
 * @returns A map with all the keys and versions.
 */
unordered_map<string, int_fast64_t> DatastoreManager::getStatus() {

    unordered_map<string, int_fast64_t> result;

    shared_lock datastoreLock(datastoreMutex);

    for (auto entry: datastore)
        result.emplace(entry.first, entry.second.second);

    return result;
}

/**
 * Initializes the DatastoreManager with the required data.
 * This function must be called before performing any operation on the datastore itself.
 * 
 * @param path Reference to the path of the datastore file to use, which must always be available.
 * @param writeCheckDelay Time to wait before checking again if there is some operation to perform on disk.
 */
void DatastoreManager::initialize(string const&path, const chrono::duration<int, milli> writeCheckDelay) {

    DatastoreManager::path = path;
    writeThread = thread([this, writeCheckDelay]() -> void { dump(writeCheckDelay); });
}

/**
 * Allows to load all the data from the file on disk.
 * If there are already some data in memory the operation is not performed, so it's suggested to call it at the beginning of the execution.
 */
void DatastoreManager::load() {

    ifstream file;
    string line;
    const regex r(R"(^\s*\"(.*?)\",\s*\"(.*?)\",\s*(.*?)$)", regex::icase | regex::optimize);
    smatch sm;

    unique_lock lock(datastoreMutex);

    file.open(path, ios::in);

    while (getline(file, line))
        if (regex_match(line, sm, r))
            datastore.emplace(sm[1], make_pair(sm[2], (int_fast64_t) stoll(sm[3])));

    file.close();
}

/**
 * Prepares the datastore for the 2PC dance, locking the entry related to the specified key so that no other thread can access it. This is a blocking method if the key isn't available.
 * It's fundamental that it's called at the very begin of the 2PC, which needs to be terminated with a commit or an abort function call.
 * 
 * @param key Reference to the key used for the 2PC.
 * @param version Reference to the version of the stored value of the required key.
 * @param uid ID of the transaction.
 * 
 * @returns Whether it was possible to perform the operation.
 */
bool DatastoreManager::prepare(string const&key, int_fast64_t &version, int_fast64_t uid) {

    unique_lock lock(datastoreMutex);

    if (owner2pc.contains(key))
        return false;

    owner2pc.emplace(key, uid);
    version = datastore.contains(key) ? datastore.at(key).second : 0;

    return true;
}

/**
 * Updates the value stored at the specified key if the version is higher than the present one. If the key isn't present, it adds it.
 * 
 * @param key Reference to the key on which performing the operation.
 * @param value Reference to the value to store.
 * @param version Version number of the stored value.
 * 
 * @return Whether the operation completed correctly.
 */
void DatastoreManager::tryUpdate(string const&key, string const&value, int_fast64_t version) {

    unordered_map<string, pair<string, int_fast64_t>>::iterator entry;

    scoped_lock pendingLock(pendingMutex);
    unique_lock lock(datastoreMutex);

    if (entry != datastore.end()) {

        pair<string, int_fast64_t> &location = entry->second;

        if (location.second < version) {
            location.first = value;
            location.second = version;
            pending = true;
        }
    } else {

        datastore.emplace(key, make_pair(value, version));
        pending = true;
    }
}
