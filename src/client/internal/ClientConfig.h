#ifndef LEADERLESS_DATASTORE_CLIENTCONFIG_H
#define LEADERLESS_DATASTORE_CLIENTCONFIG_H

#include <string>
#include <unordered_map>
#include <vector>

class ClientConfig {

    public:
    ClientConfig(int argc, char *argv[]);
    ClientConfig(const std::string &configPath);

    private:
    std::vector<std::string> servers;

    public:
    std::vector<std::string> getServers() const;

    private:
    std::string getKey(std::string &key);
    void initialize(const std::string &configPath);
    std::unordered_map<std::string, std::string> parseArguments(int argc, char *argv[]);
};

#endif
