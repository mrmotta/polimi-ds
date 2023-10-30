//
// Created by Niccolò Nicolosi on 20/03/23.
//

#include <memory>
#include <string>
#include <random>
#include <gtest/gtest.h>
#include "../src/client//internal/ClientConfig.h"
#include "../src/server/internal/ServerConfig.h"
#include "../src/server/internal/Replica.h"
#include "../src/client//internal/Client.h"

#define serverConfigPath "../tests/config/server.yaml"
#define clientConfigPath "../tests/config/client.yaml"
#define RANDOM_ENGINE linear_congruential_engine<uint_fast32_t, 48271, 0, 2147483647>
#define RANDOM_GENERATOR mt19937

using namespace std;

class ClientServerTests : public testing::Test {

public:
    RANDOM_ENGINE rng;
    RANDOM_GENERATOR rgen;

    int replicaNum, clientNum;
    vector<unique_ptr<ServerConfig>> serverConfigs;
    unique_ptr<ClientConfig> clientConfig;

    vector<shared_ptr<Replica>> replicas;
    vector<shared_ptr<Client>> clients;

    ClientServerTests()
    : rng(default_random_engine {random_device {}()}),
      rgen(random_device {}()) {

        clientConfig = std::make_unique<ClientConfig>(clientConfigPath);

        clientNum = 1;
        replicaNum = (int) clientConfig->getServers().size();

        for(int i = 0; i < replicaNum; i++)
            serverConfigs.emplace_back(make_unique<ServerConfig>(
                    serverConfigPath, i, "../tests/data/", false));
    }

    //code to run before each test
    void SetUp() override {

        for(const auto &config : serverConfigs)
            replicas.emplace_back(make_unique<Replica>(*config));

        for(int i = 0; i < clientNum; i++)
            clients.emplace_back(make_unique<Client>(*clientConfig));
    }

    //code to run after each test;
    void TearDown() override {

        while(!replicas.empty()) {

            replicas.at(replicas.size() - 1)->shutdown();
            replicas.pop_back();
        }

        clients.clear();
    }

    string randomString(int maxLength) {

        uniform_int_distribution<> distribution(1, maxLength);
        size_t length = distribution(rgen);

        auto randChar = [this]() -> char {

            uniform_int_distribution<> distribution(0, INT_MAX);
            const char charset[] =
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[distribution(rgen) % max_index];
        };

        std::string str(length,0);
        std::generate_n(str.begin(), length, randChar);
        return str;
    }
};

TEST_F(ClientServerTests, multipleMessages) {

    EXPECT_NO_THROW(

            for(auto replica : replicas)
                replica->start(true);

            for(int i = 0; i < 100; i++)
                clients.at(0)->put(randomString(64), randomString(256));
    );
}

TEST_F(ClientServerTests, multipleReplicas) {

    EXPECT_NO_THROW(

            for(const auto &replica : replicas)
                replica->start(true);

            for(int i = 0; i < 10; i++)
                clients[0]->put(randomString(64), randomString(256));
    );
}