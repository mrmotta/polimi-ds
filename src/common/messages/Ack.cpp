//
// Created by Pierluigi Negro on 3/22/23.
//

#include <iostream>
#include <string>
#include <utility>
#include "../Message.h"

using namespace std;

Message::Ack::Ack(string info) : Message(ACK) {
    this->info = std::move(info);
}

shared_ptr<Message> Message::Ack::toMessage(const string &messageString) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::ack))
        return make_shared<Ack>(sm[1]);

    throw RegexMismatchException("Mismatch reading message Ack");
}

string Message::Ack::toString() const {
    return "ACK; " + info;
}

string Message::Ack::getInfo() {
    return info;
}

void Message::Ack::manage() const {
    cout << info << endl;
}
