#include <iostream>
#include "ClientOperation.h"
#include "internal/ClientConfig.h"
#include "internal/Client.h"

using namespace std;

int main(int argc, char *argv[]) {

    cout << endl << "\033[;1mQUORUM-BASED REPLICATED DATASTORE\033[0m\n\n";

    ClientConfig config(argc, argv);
    Client client(config);

    bool exit = false;
    string input;
    ClientOperation operation;

    cout << "To use the datastore, write:\n"
            "- \033[;4mget \"k\"\033[0m to retrieve the value associated to k\n"
            "- \033[;4mput \"k\" \"v\"\033[0m to insert/update the value v associated to key k\n"
            "- \033[;4mexit\033[0m to terminate\n" << endl;

    do {
        cout << "\033[94maction\033[0m> ";
        if (!getline(cin, input))
            break;
        operation.parse(input);
        switch (operation.getType()) {
            case ClientOperation::ClientOperationType::GET:
                client.get(operation.getKey());
                break;
            case ClientOperation::ClientOperationType::PUT:
                client.put(operation.getKey(), operation.getValue());
                break;
            case ClientOperation::ClientOperationType::EXIT:
                exit = true;
                break;
            default:
                cout << "Unrecognised operation, please try again.\n";
        }
        cout << "\n";
    } while (!exit);

    return 0;
}
