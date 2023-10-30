#ifndef LEADERLESS_DATASTORE_SERVERCONFIG_H
#define LEADERLESS_DATASTORE_SERVERCONFIG_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#define CONFIG_FILE "../config/server.yaml"
#define DATASTORE_PATH "../data/"

class ServerConfig {

    public:
    ServerConfig(int argc, char *argv[]);
    ServerConfig(const std::string &configPath, int replicaNumber, const std::string &datastorePath, bool loadDatastore);

    private:
    std::string datastorePath;
    bool loadDatastore;
    int port;
    int readQuorum = 0;
    std::vector<std::string> replicas;
    int replicaId = 0;
    int writeQuorum = 0;

    public:
    std::string getDatastorePath() const;
    int getPort() const;
    int getReadQuorum() const;
    std::vector<std::string> getReplicas();
    int getReplicasAmount() const;
    int getReplicaId() const;
    int getWriteQuorum() const;
    bool loadDatastoreFromFile() const;

    private:
    std::string getKey(std::string &key);
    std::string loop(std::ifstream &file);
    void initialize(const std::string &configPath, int replicaNumber, const std::string &datastorePath, bool loadDatastore);
    std::unordered_map<std::string, std::string> parseArguments(int argc, char *argv[]);
    void processConfig(std::ifstream &file);
    void processServers(std::ifstream &file, int replicaNumber);
};

#endif
