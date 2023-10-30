#include <iostream>
#include "ClientOperation.h"

using namespace std;

const regex ClientOperation::regexExit(R"(^\s*(exit)\s*$)", regex::icase|regex::optimize);
const regex ClientOperation::regexGet(R"(^\s*(get)\s*\"(.*)\"\s*$)", regex::icase|regex::optimize);
const regex ClientOperation::regexPut(R"(^\s*(put)\s*\"(.*)\"\s*\"(.*)\"\s*$)", regex::icase|regex::optimize);

void ClientOperation::parse(string &line) {

    vector<string> input = split(line);

    type = WRONG;
    key = "";
    value = "";

    switch (input.size()) {
        case 1:
            if (input.front() == "exit")
                type = EXIT;
            break;
        case 2:
            if (input.front() == "get") {
                type = GET;
                key = input[1];
            }
            break;
        case 3:
            if (input.front() == "put") {
                type = PUT;
                key = input[1];
                value = input[2];
            }
    }
}

string ClientOperation::getKey() const                                { return key;   }
ClientOperation::ClientOperationType ClientOperation::getType() const { return type;  }
string ClientOperation::getValue() const                              { return value; }

vector<string> ClientOperation::split(string &line) {

    vector<string> result;
    string st;
    smatch sm;

    if (regex_match(line, sm, regexGet)) {
        result.push_back(string(sm[1]));
        st = string(sm[2]);
        result.push_back(st);
    } else if (regex_match(line, sm, regexPut)) {
        result.push_back(string(sm[1]));
        st = string(sm[2]);
        st.erase(remove(st.begin(), st.end(), '"'), st.end());
        result.push_back(st);
        st = string(sm[3]);
        st.erase(remove(st.begin(), st.end(), '"'), st.end());
        result.push_back(st);
    } else if (regex_match(line, sm, regexExit))
        result.push_back(string(sm[1]));

    return result;
}
