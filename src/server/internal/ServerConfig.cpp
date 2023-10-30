#include <iostream>
#include "ServerConfig.h"

using namespace std;

ServerConfig::ServerConfig(int argc, char *argv[]) {

    unordered_map<string, string> arguments = parseArguments(argc, argv);
    int replicaId;
    bool loadDatastore;

    if (!arguments.contains("replica"))
        throw runtime_error("You need to specify the number of the replica (starting from 0).");

    try {
        replicaId = stoi(arguments.at("replica"));
    } catch (...) {
        throw runtime_error("The replica number must be an integer.");
    }

    if (arguments.contains("load-datastore"))
        try {
            loadDatastore = stoi(arguments.at("load-datastore"));
        } catch (...) {
            loadDatastore = true;
        }
    else
        loadDatastore = true;

    initialize(
        arguments.contains("config") ? arguments.at("config") : CONFIG_FILE,
        replicaId,
        arguments.contains("datastore") ? arguments.at("datastore") : DATASTORE_PATH + to_string(replicaId) + ".db",
        loadDatastore
    );
}

ServerConfig::ServerConfig(const string &configPath, int replicaId, const string &datastorePath, bool loadDatastore) {
    initialize(configPath, replicaId, datastorePath + to_string(replicaId) + ".db", loadDatastore);
}

string ServerConfig::getDatastorePath() const {
    return datastorePath;
}

string ServerConfig::getKey(string &key) {
    if (key == "c")
        return "config";
    if (key == "d")
        return "datastore";
    if (key == "l")
        return "load";
    if (key == "r")
        return "replica";
    return key;
}

int ServerConfig::getPort() const {
    return port;
}

int ServerConfig::getReadQuorum() const {
    return readQuorum;
}

vector<string> ServerConfig::getReplicas() {
    return replicas;
}

int ServerConfig::getReplicasAmount() const{
    return replicas.size();
}

int ServerConfig::getReplicaId() const {
    return replicaId;
}

int ServerConfig::getWriteQuorum() const {
    return writeQuorum;
}

void ServerConfig::initialize(const string &configPath, int replicaId, const string &datastorePath, bool loadDatastore) {

    ifstream file;
    bool updatedRQ = false, updatedWQ = false;

    this->replicaId = replicaId;
    file.open(configPath, ios::in);

    if (!file.is_open())
        throw runtime_error("Config file not found.");

    string tmp = loop(file);
    if (file.eof())
        throw runtime_error("Config file is incomplete.");

    if (tmp == "config:")
        processConfig(file);
    else if (tmp == "servers:")
        processServers(file, replicaId);

    tmp = loop(file);
    if (file.eof())
        throw runtime_error("Config file is incomplete.");

    if (tmp == "config:")
        processConfig(file);
    else if (tmp == "servers:")
        processServers(file, replicaId);

    file.close();

    if (replicas.size() == 0)
        throw runtime_error("No replica was specified in the config file.");

    if (replicaId < 0 || replicaId > (int) replicas.size() + 1)
        throw runtime_error("There is no replica for the specified number.");

    if (writeQuorum <= ((int) replicas.size() + 1) / 2) {
        writeQuorum = (replicas.size() + 1) / 2 + 1;
        updatedWQ = true;
    } else if (writeQuorum > (int) replicas.size() + 1) {
        writeQuorum = replicas.size() + 1;
        updatedWQ = true;
    }

    if (updatedWQ)
        cout << "\e[33mWarning\e[0m: write quorum is invalid, it has been changed to " << writeQuorum << "." << endl;

    if (readQuorum <= (int) replicas.size() + 1 - writeQuorum) {
        readQuorum = replicas.size() - writeQuorum + 2;
        updatedRQ = true;
    } else if (readQuorum > (int) replicas.size() + 1) {
        readQuorum = replicas.size() + 1;
        updatedRQ = true;
    }

    if (updatedRQ)
        cout << "\e[33mWarning\e[0m: read quorum is invalid, it has been changed to " << readQuorum << "." << endl;

    if (updatedRQ || updatedWQ)
        cout << endl;

    this->datastorePath = datastorePath;

    file.open(datastorePath, ios::in);
    if (!file.is_open())
        file.open(datastorePath, ios::out | ios::trunc);
    file.close();

    this->loadDatastore = loadDatastore;
}

bool ServerConfig::loadDatastoreFromFile() const {
    return loadDatastore;
}

string ServerConfig::loop(ifstream &file) {
    string tmp;
    do file >> tmp;
    while (tmp != "config:" && tmp != "servers:" && !file.eof());
    return tmp;
}

unordered_map<string, string> ServerConfig::parseArguments(int argc, char *argv[]) {

    unordered_map<string, string> arguments;
    string key;
    bool foundKey = false;

    for (int i = 1; i < argc; i ++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            key = argv[i];
            key = key.substr(argv[i][1] == '-' ? 2 : 1, key.size());
            foundKey = key.size();
        } else if (foundKey) {
            arguments.emplace(getKey(key), argv[i]);
            foundKey = false;
        }
    }

    return arguments;
}

void ServerConfig::processConfig(ifstream &file) {

    string tmp;

    for (int i = 0; i < 2; i ++) {

        do {
            file >> tmp;
            if (tmp != "-")
                return;
            file >> tmp;
        } while (tmp != "read_quorum:" && tmp != "write_quorum:" && !file.eof());

        if (file.eof())
            throw runtime_error("Config file is incomplete.");

        if (tmp == "read_quorum:") {
            file >> tmp;
            try {
                readQuorum = stoi(tmp);
            } catch (...) {
                readQuorum = 0;
            }
        } else if (tmp == "write_quorum:") {
            file >> tmp;
            try {
                writeQuorum = stoi(tmp);
            } catch (...) {
                readQuorum = 0;
            }
        }
    }
}

void ServerConfig::processServers(ifstream &file, int replicaId) {

    string tmp;
    int index = 0;

    file >> tmp;
    while (tmp == "-") {
        file >> tmp;
        if (index != replicaId)
            replicas.emplace_back(tmp);
        else
            port = stoi(tmp.substr(tmp.find(':') + 1, tmp.length()));

        file >> tmp;
        index ++;
    }
}
