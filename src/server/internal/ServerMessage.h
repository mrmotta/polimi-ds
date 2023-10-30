//
// Created by Pierluigi Negro on 3/26/23.
//

#ifndef LEADERLESS_DATASTORE_SERVERMESSAGE_H
#define LEADERLESS_DATASTORE_SERVERMESSAGE_H

#include "Coordinator.h"
#include "Participant.h"
#include "Replica.h"
#include "../../common/Message.h"

class ServerMessage : public Message {

    private:
    Replica &replica;

    protected:
    ServerMessage(MessageType msgType, Replica &replica);
    Replica &getReplica();

    public:
    class Fetch;
    class FetchedValue;
    class Get;
    class GlobalCommit;
    class GlobalAbort;
    class Prepare;
    class Put;
    class Repair;
    class Sync;
    class Update;
    class VoteAbort;
    class VoteCommit;

    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::Fetch final : public ServerMessage {
    private:
    std::string key;
    ReplicaConnectionHandler::Connection *connection;

    public:
    Fetch(std::string key, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    void setConnection(ReplicaConnectionHandler::Connection *connection);
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::FetchedValue final : public ServerMessage {
    private:
    std::string key, value;
    int_fast64_t timestamp;
    bool valid;

    public:
    FetchedValue(std::string key, Replica &replica);
    FetchedValue(std::string key, std::string value, int_fast64_t timestamp, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    static std::shared_ptr<Message> toMessage(const std::string& messageString, Replica &replica);
};

class ServerMessage::Get final : public ServerMessage {
    private:
    std::string key;
    ReplicaConnectionHandler::Connection *connection;

    public:
    Get(std::string key, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    void setConnection(ReplicaConnectionHandler::Connection *connection);
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::GlobalCommit final : public ServerMessage {
    private:
    std::string key, value;
    int_fast64_t uid;
    int timestamp;

    public:
    GlobalCommit(int_fast64_t uid, std::string key, std::string value, int timestamp, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::GlobalAbort final : public ServerMessage {
    private:
    std::string key;
    int_fast64_t uid;

    public:
    GlobalAbort(int_fast64_t uid, std::string key, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::Prepare final : public ServerMessage {
    private:
    std::string key, value;
    int_fast64_t uid;
    ReplicaConnectionHandler::Connection *connection;

    public:
    Prepare(int_fast64_t uid, std::string key, std::string value, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    void setConnection(ReplicaConnectionHandler::Connection *connection);
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::Put final : public ServerMessage {

    private:
    std::string key, value;
    ReplicaConnectionHandler::Connection *connection;

    public:
    Put(std::string key, std::string value, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    void setConnection(ReplicaConnectionHandler::Connection *connection);
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::Repair final : public ServerMessage {
    private:
    std::string key, value;
    int_fast64_t timestamp;
    ReplicaConnectionHandler::Connection *connection;

    public:
    Repair(std::string key, std::string value, int_fast64_t timestamp, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    void setConnection(ReplicaConnectionHandler::Connection *connection);
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::Sync final : public ServerMessage {
    private:
    std::unordered_map<std::string, int_fast64_t> entries;
    ReplicaConnectionHandler::Connection *connection;

    public:
    Sync(const std::unordered_map<std::string, int_fast64_t>& entries, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    void setConnection(ReplicaConnectionHandler::Connection *connection);
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::Update final : public ServerMessage {
    private:
    std::unordered_map<std::string, std::pair<std::string, int_fast64_t>> newValues;

    public:
    Update(const std::unordered_map<std::string, std::pair<std::string, int_fast64_t>> &newValues, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::VoteAbort final : public ServerMessage {
    private:
    std::string key;
    int_fast64_t uid;

    public:
    VoteAbort(int_fast64_t uid, std::string key, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

class ServerMessage::VoteCommit final : public ServerMessage {
    private:
    std::string key;
    int_fast64_t uid;
    int_fast64_t timestamp;

    public:
    VoteCommit(int_fast64_t uid, std::string key, int_fast64_t timestamp, Replica &replica);
    std::string toString() const override;
    void manage() const override;
    static std::shared_ptr<Message> toMessage(const std::string &messageString, Replica &replica);
};

#endif //LEADERLESS_DATASTORE_SERVERMESSAGE_H
