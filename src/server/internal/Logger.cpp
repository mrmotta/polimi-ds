#include "Logger.h"

using namespace std;

Logger::~Logger() {
    close();
}

void Logger::close() {
    scoped_lock lock{mutex};
    if (!initialized.load())
        return;
    if (file.is_open())
        file.close();
}

vector<string> Logger::initialize(string &path, bool reset) {
    scoped_lock lock{mutex};
    if (initialized.load())
        return vector<string>();
    ifstream file;
    vector<string> operations;
    string tmp;
    Logger::path = path;
    if (!reset) {
        file.open(path);
        while (!file.eof()) {
            getline(file, tmp);
            operations.emplace_back(tmp);
        }
        file.close();
        file.open(path);
    } else
        file.open(path);
    initialized.store(true);
    return operations;
}

void Logger::reset() {
    scoped_lock lock{mutex};
    if (!initialized.load())
        return;
    if (file.is_open())
        file.close();
    file.open(path);
}

void Logger::write(string &message) {
    scoped_lock lock{mutex};
    if (!initialized.load())
        return;
    if (file.is_open())
        file << message << endl;
}
