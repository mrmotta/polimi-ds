//
// Created by Pierluigi Negro on 3/26/23.
//

#ifndef LEADERLESS_DATASTORE_CLIENTMESSAGE_H
#define LEADERLESS_DATASTORE_CLIENTMESSAGE_H


#include "../../common/Message.h"

class ClientMessage : Message {

public:
    class ClientGet;
    class ClientPut;
};

class ClientMessage::ClientGet final : public Message {
private:
    std::string key;

public:
    explicit ClientGet(std::string key);
    std::string toString() const override;
    void manage() const override {};
};

class ClientMessage::ClientPut final : public Message {

private:
    std::string key, value;

public:
    ClientPut(std::string key, std::string value);
    std::string toString() const override;
    void manage() const override {};
};

#endif //LEADERLESS_DATASTORE_CLIENTMESSAGE_H
