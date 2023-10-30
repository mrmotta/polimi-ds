//
// Created by Pierluigi Negro on 3/12/23.
//

#include <utility>
#include "../ClientMessage.h"

using namespace std;

ClientMessage::ClientPut::ClientPut(string key, string value) : Message(PUT) {
    this->key = std::move(key);
    this->value = std::move(value);
}

string ClientMessage::ClientPut::toString() const {
    return "PUT; k:\"" + key + "\", v:\"" + value + "\"";
}
