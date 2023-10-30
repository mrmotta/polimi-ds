//
// Created by Pierluigi Negro on 06/03/23.
//

#include <string>
#include <utility>
#include "../ClientMessage.h"

using namespace std;

ClientMessage::ClientGet::ClientGet(string key) : Message(GET) {
    this->key = std::move(key);
}

string ClientMessage::ClientGet::toString() const {
    return "GET; k:\"" + key + "\"";
}
