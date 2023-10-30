//
// Created by Pierluigi Negro on 06/03/23.
//

#ifndef LEADERLESS_DATASTORE_MESSAGE_H
#define LEADERLESS_DATASTORE_MESSAGE_H

#include <string>
#include <vector>
#include <memory>
#include "Regex.h"
#include "RegexMismatchException.h"

class Message {

    public:
    typedef enum {
        ACK,
        NACK,
        PUT,
        GET,
        VALUE,
        FETCH,
        FETCHED_VALUE,
        PREPARE,
        VOTE_COMMIT,
        VOTE_ABORT,
        PREPARE_COMMIT,
        GLOBAL_ABORT,
        READY_COMMIT,
        GLOBAL_COMMIT,
        REPAIR,
        SYNC,
        UPDATE
    } MessageType;

    class Ack;
    class Nack;
    class Value;

    private:
    const MessageType messageType;

    protected:
    explicit Message(MessageType msgType);

    public:
    virtual std::string toString() const = 0;
    virtual void manage() const = 0;
    MessageType getType() const;

    static MessageType toMessageType(const std::string &messageTypeString);
    static std::shared_ptr<Message> toMessage(const std::string &messageString);
};

class Message::Ack final : public Message {
    private:
    std::string info;

    public:
    explicit Ack(std::string info);
    std::string toString() const override;
    void manage() const override;
    std::string getInfo();
    static std::shared_ptr<Message> toMessage(const std::string& messageString);
};

class Message::Nack final : public Message {
    private:
    std::string info;

    public:
    explicit Nack(std::string info);
    std::string toString() const override;
    void manage() const override;
    std::string getInfo();
    static std::shared_ptr<Message> toMessage(const std::string& messageString);
};

class Message::Value final : public Message {
    private:
    bool valid;
    std::string value;

    public:
    explicit Value();
    explicit Value(std::string value);
    std::string toString() const override;
    void manage() const override;
    bool found() const;
    std::string getValue() const;
    static std::shared_ptr<Message> toMessage(const std::string& messageString);
};

#endif //LEADERLESS_DATASTORE_MESSAGE_H
