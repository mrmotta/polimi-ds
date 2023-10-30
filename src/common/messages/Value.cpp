//
// Created by Pierluigi Negro on 3/22/23.
//

#include <iostream>
#include <string>
#include <utility>
#include "../Message.h"

using namespace std;

Message::Value::Value() : Message(VALUE) {
    this->valid = false;
}

Message::Value::Value(string value) : Message(VALUE) {
    this->value = std::move(value);
    this->valid = true;
}

shared_ptr<Message> Message::Value::toMessage(const string &messageString) {

    smatch sm;
    if (regex_match(messageString, sm, Regex::value)) {
        if (sm[1] == "null")
            return make_shared<Value>();
        return make_shared<Value>(sm[1]);
    }

    throw RegexMismatchException("Mismatch reading message Value");
}

string Message::Value::toString() const {
    return "VALUE; " + (valid ? "\"" + value + "\"" : "null");
}

bool Message::Value::found() const {
    return valid;
}

string Message::Value::getValue() const {
    return value;
}

void Message::Value::manage() const {
    if (valid)
        cout << "The retrieved value is:\n" << value << endl;
    else
        cout << "The searched key wasn't found." << endl;
}
