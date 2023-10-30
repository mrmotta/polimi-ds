from enum import Enum
import yaml
import os
import pathlib
import platform
import signal
import subprocess
import sys



kwargs = {}
if platform.system() == 'Windows':
    # from msdn [1]
    CREATE_NEW_PROCESS_GROUP = 0x00000200  # note: could get it from subprocess
    DETACHED_PROCESS = 0x00000008          # 0x8 | 0x200 == 0x208
    kwargs.update(creationflags=DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP)
elif sys.version_info < (3, 2):  # assume posix
    kwargs.update(preexec_fn=os.setsid)
else:  # Python 3.2+ and Unix
    kwargs.update(start_new_session=True)



class OperationType(Enum):
    EXIT = 0
    START = 1
    STOP = 2
    WRONG = 3



class Operation:

    type = OperationType.WRONG
    k = 0

    def parse(self: any, input: str):
        try:
            if input.startswith('start'):
                self.type = OperationType.START
                self.k = int(input[6:])
            elif input.startswith('stop'):
                self.type = OperationType.STOP
                self.k = int(input[5:])
            elif input == 'exit':
                self.type = OperationType.EXIT
            else:
                self.type = OperationType.WRONG
        except Exception:
            self.type = OperationType.WRONG



def startAll(serverPath: int, startValue: int, endValue: int, configPath: str, dataPath: str):
    print('Starting servers...')
    for index in range(startValue, endValue):
        pids[index] = start(serverPath, index, configPath, dataPath)
    print('Done')



def start(serverPath: int, index: int, configPath: str, dataPath: str):
    data = os.path.join(dataPath, str(index) + '.db')
    return subprocess.Popen(
        [serverPath, '-r', str(index), '-c', configPath, '-d', data, '-l', '1'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        **kwargs,
        shell=platform.system() == 'Windows'
    ).pid



def stop(pid: int):
    try:
        os.kill(pid, signal.SIGTERM)
    except Exception:
        print('Can\'t stop server instance with pid', pid)



if __name__ == '__main__':
    path = pathlib.Path(__file__).parent.resolve()
    configPath = os.path.join(path, 'config', 'server.yaml')
    dataPath = os.path.join(path, 'data')
    serverPath = os.path.join(path, 'build', 'server.exe')

    # if datapath doesn't exist, create it
    if not os.path.exists(dataPath):
        os.makedirs(dataPath)
    
    with open(configPath, 'r') as file:
        try:
            config = yaml.safe_load(file)
        except yaml.YAMLError as exc:
            config = {}

    servers = config['servers']

    pids = {}

    print('\n\033[;1mQUORUM-BASED LEADERLESS DATASTORE\033[0m\n')

    try:
        if len(sys.argv) == 3:
            startValue = int(sys.argv[1])
            endValue = int(sys.argv[2]) + 1
        if startValue > endValue:
            tmp = startValue
            startValue = endValue
            endValue = tmp
    except Exception:
        startValue = 0
        endValue = len(servers)

    startAll(serverPath, startValue, endValue, configPath, dataPath)

    print('\nThis application allows to:')
    print('- \033[;4mstart k\033[0m for starting server k (starting from 0)')
    print('- \033[;4mstop k\033[0m for stopping server k (starting from 0)')
    print('- \033[;4mexit\033[0m for terminating\n')

    action = input('\033[94maction\033[0m> ')
    operation = Operation()
    operation.parse(action)
    while not operation.type == OperationType.EXIT:
        if not operation.type == OperationType.WRONG:
            if operation.type == OperationType.START:
                if operation.k in pids and pids[operation.k] == None:
                    pids[operation.k] = start(serverPath, operation.k, configPath, dataPath)
                else:
                    print('Server not present or already running.')
            elif operation.type == OperationType.STOP:
                if operation.k in pids and not pids[operation.k] == None:
                    stop(pids[operation.k])
                    pids[operation.k] = None
                else:
                    print('Server doesn\'t exist.')
        else:
            print('Unrecognised operation, please try again.')
        action = input('\n\033[94maction\033[0m> ')
        operation.parse(action)

    print('\nStopping all servers...')
    for pid in pids.values():
        if not pid == None:
            stop(pid)
    print('Done\n')
