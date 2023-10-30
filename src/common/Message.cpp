//
// Created by Pierluigi Negro on 06/03/23.
//

#include "Message.h"

using namespace std;

Message::Message(MessageType  msgType)
: messageType(msgType) {}

Message::MessageType Message::getType() const {
    return messageType;
}

Message::MessageType Message::toMessageType(const std::string &messageTypeString) {

    if (messageTypeString == "ACK")              return ACK;
    if (messageTypeString == "NACK")             return NACK;
    if (messageTypeString == "GET")              return GET;
    if (messageTypeString == "VALUE")            return VALUE;
    if (messageTypeString == "PUT")              return PUT;
    if (messageTypeString == "FETCH")            return FETCH;
    if (messageTypeString == "FETCHED_VALUE")    return FETCHED_VALUE;
    if (messageTypeString == "PREPARE")          return PREPARE;
    if (messageTypeString == "VOTE_COMMIT")      return VOTE_COMMIT;
    if (messageTypeString == "VOTE_ABORT")       return VOTE_ABORT;
    if (messageTypeString == "PREPARE_COMMIT")   return PREPARE_COMMIT;
    if (messageTypeString == "GLOBAL_ABORT")     return GLOBAL_ABORT;
    if (messageTypeString == "READY_COMMIT")     return READY_COMMIT;
    if (messageTypeString == "GLOBAL_COMMIT")    return GLOBAL_COMMIT;
    if (messageTypeString == "REPAIR")           return REPAIR;
    if (messageTypeString == "SYNC")             return SYNC;
    if (messageTypeString == "UPDATE")           return UPDATE;

    throw RegexMismatchException("No matching message type");
}

shared_ptr<Message> Message::toMessage(const string &messageString) {

    smatch sm;
    string message = messageString.substr(0, messageString.size() - 1);

    if (regex_match(messageString, sm, Regex::split)) {
        switch (toMessageType(sm[1])) {
            case ACK:               return Ack::toMessage(message);
            case NACK:              return Nack::toMessage(message);
            case VALUE:             return Value::toMessage(message);
            default:                throw runtime_error("Unable to parse message");
        }
    }

    throw RegexMismatchException("Invalid message format");
}
