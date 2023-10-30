//
// Created by Pierluigi Negro on 3/22/23.
//

#include <iostream>
#include <string>
#include <utility>
#include "../Message.h"

using namespace std;

Message::Nack::Nack(string info) : Message(NACK) {
    this->info = std::move(info);
}

shared_ptr<Message> Message::Nack::toMessage(const string& messageString) {
    smatch sm;
    if (regex_match(messageString, sm, Regex::nack))
        return make_shared<Nack>(sm[1]);

    throw RegexMismatchException("Mismatch reading message Nack");
}

string Message::Nack::toString() const {
    return "NACK; " + info;
}

string Message::Nack::getInfo(){
    return info;
}

void Message::Nack::manage() const{
    cout << info << endl;
}
