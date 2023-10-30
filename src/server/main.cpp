#include <iostream>
#include <csignal>
#include "internal/ServerConfig.h"
#include "ServerOperation.h"
#include "internal/Replica.h"

using namespace std;

Replica *replica;

void terminationHandler(int sig) {

    if(replica->isAlive())
        replica->shutdown();
}

int main(int argc, char *argv[]) {

    cout << endl << "\033[;1mQUORUM-BASED REPLICATED DATASTORE\033[0m\n\n";

    #if defined(_WIN32) || defined(_WIN64)
        signal(SIGINT, terminationHandler);
        signal(SIGTERM, terminationHandler);
        signal(SIGABRT, terminationHandler);
        signal(SIGBREAK, terminationHandler);
        signal(SIGSEGV, terminationHandler);
    #else
        struct sigaction sa = {};
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = terminationHandler;
        if(sigaction(SIGINT, &sa, nullptr) == -1)
            throw runtime_error("Couldn't set SIGINT callback.");
        if(sigaction(SIGTERM, &sa, nullptr) == -1)
            throw runtime_error("Couldn't set SIGTERM callback.");
        if(sigaction(SIGABRT, &sa, nullptr) == -1)
            throw runtime_error("Couldn't set SIGABRT callback.");
        if(sigaction(SIGSEGV, &sa, nullptr) == -1)
            throw runtime_error("Couldn't set SIGSEGV callback.");
    #endif

    ServerConfig config = ServerConfig(argc, argv);
    replica = new Replica(config);
    replica->start(true);

    bool exit = false;
    string input;
    ServerOperation operation;

    cout << "\nTo manage the replica, write:\n"
            "- \033[;4mdebug\033[0m to toggle debug messages\n"
            "- \033[;4mexit\033[0m to terminate\n" << endl;

    do {
        if (!getline(cin, input))
            break;
        operation.parse(input);
        switch (operation.getType()) {
            case ServerOperation::ServerOperationType::DEBUG:
                replica->setDebug(!replica->isDebug());
                if (replica->isDebug())
                    cout << "Toggled debug status: debug = true.\n";
                else
                    cout << "Toggled debug status: debug = false.\n";
                break;
            case ServerOperation::ServerOperationType::EXIT:
                exit = true;
                break;
            default:
                cout << "Unrecognised operation, please try again.\n";
        }

        cout << "\n";
    } while (!exit);

    delete replica;
    return 0;
}
