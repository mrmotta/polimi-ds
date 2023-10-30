#include <fstream>
#include <iostream>
#include "ClientConfig.h"

#define CONFIG_FILE "../config/client.yaml"

using namespace std;

ClientConfig::ClientConfig(int argc, char *argv[]) {
    unordered_map<string, string> arguments = parseArguments(argc, argv);
    initialize(arguments.contains("config") ? arguments.at("config") : CONFIG_FILE);
}

ClientConfig::ClientConfig(const string &configPath) {
    initialize(configPath);
}

string ClientConfig::getKey(string &key) {
    if (key == "c")
        return "config";
    return key;
}

vector<string> ClientConfig::getServers() const {
    return servers;
}

void ClientConfig::initialize(const string &configPath) {

    ifstream file;

    file.open(configPath, ios::in);

    if (!file.is_open())
        throw runtime_error("Config file not found.");

    string tmp;

    do file >> tmp;
    while (tmp != "servers:" && !file.eof());

    if (file.eof())
        throw runtime_error("Config file is incomplete.");

    file >> tmp;
    while (tmp == "-") {
        file >> tmp;
        servers.emplace_back(tmp);
        file >> tmp;
    }

    file.close();
}

unordered_map<string, string> ClientConfig::parseArguments(int argc, char *argv[]) {

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
