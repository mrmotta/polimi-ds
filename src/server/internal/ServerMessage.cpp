//
// Created by Pierluigi Negro on 3/26/23.
//

#include "ServerMessage.h"

using namespace std;

ServerMessage::ServerMessage(MessageType msgType, Replica &replica)
: Message(msgType), replica(replica) {}

Replica &ServerMessage::getReplica() {
    return replica;
}

shared_ptr<Message> ServerMessage::toMessage(const string &messageString, Replica &replica) {

    try {
        return Message::toMessage(messageString);
    } catch(runtime_error) {

        smatch sm;
        string message = messageString.substr(0, messageString.size() - 1);

        if (regex_match(messageString, sm, Regex::split)) {
            switch (toMessageType(sm[1])) {
                case GET:               return Get::toMessage(message, replica);
                case PUT:               return Put::toMessage(message, replica);
                case FETCH:             return Fetch::toMessage(message, replica);
                case FETCHED_VALUE:     return FetchedValue::toMessage(message, replica);
                case PREPARE:           return Prepare::toMessage(message, replica);
                case VOTE_COMMIT:       return VoteCommit::toMessage(message, replica);
                case VOTE_ABORT:        return VoteAbort::toMessage(message, replica);
                case GLOBAL_ABORT:      return GlobalAbort::toMessage(message, replica);
                case GLOBAL_COMMIT:     return GlobalCommit::toMessage(message, replica);
                case REPAIR:            return Repair::toMessage(message, replica);
                case SYNC:              return Sync::toMessage(message, replica);
                case UPDATE:            return Update::toMessage(message, replica);
                default:                throw runtime_error("Unable to parse message");
            }
        }
        throw RegexMismatchException("Invalid message format");
    }
}
