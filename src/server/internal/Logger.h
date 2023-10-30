#ifndef LEADERLESS_DATASTORE_LOGGER_H
#define LEADERLESS_DATASTORE_LOGGER_H

#include <atomic>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <mutex>

class Logger {

    public:
    Logger() {}
    ~Logger();

    public:
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

    private:
    std::ofstream file;
    std::atomic<bool> initialized = false;
    std::mutex mutex;
    std::string path;

    public:
    std::vector<std::string> initialize(std::string &path, bool reset);
    void reset();
    void write(std::string &message);

    private:
    void close();
};

#endif
