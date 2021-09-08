import os
import socket
import random
import logging
from protocol import Request, Response, OP_CODES, STATUS_CODES
from exceptions import BackupError, RestoreError, RemoveError, ListFilesError
from functools import wraps

logging.basicConfig(level=logging.INFO, format="[%(levelname)s]: %(message)s")

SERVER_INFO_FILE = "server.info"
BACKUP_FILE = "backup.info"

class Client(object):
    """
    Represents an client.
    """
    
    VERSION = 1
    def __init__(self, user_id, server_ip, server_port):
        self.user_id = user_id
        self.server_ip = server_ip
        self.server_port = server_port
        self._socket = None
        logging.info(f"Client '{user_id}' to ({server_ip}, {server_port}) was created")

    def connect(self):
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        logging.debug(f"Conencting to ({self.server_ip}, {self.server_port})")
        self._socket.connect((self.server_ip, self.server_port))
        logging.debug(f"Connected successfully! Using socket {self._socket.getsockname()}")

    
    def is_connected(self):
        return bool(self._socket)

    def close(self):
        logging.debug(f"Closing socket - '{self._socket.getsockname()}")
        self._socket.close()
        self._socket = None

    def on_connect(inner):
        @wraps(inner)
        def wrapper(self, *args, **kwargs):
            if self.is_connected():
                self.close()
            self.connect()
            ret = inner(self, *args, **kwargs)
            self.close()
            return ret
        return wrapper

    @on_connect
    def backup_file(self, file_path):
        if not os.path.exists(file_path):
            raise FileNotFoundError(f"{file_path} doesn't exist")
        logging.info(f"Backing up '{file_path}'...")
        response = self._send_request(OP_CODES.BACKUP_FILE, file_path)
        if response.status == STATUS_CODES.BACKUP_OR_REMOVE_SUCCESS:
            logging.info(f"{file_path} backed up successfully!")
        else:
            err = BackupError(response.status, file_path)
            logging.warning(str(err))
            raise err

    @on_connect
    def restore_file(self, filename, path_to_restore):
        logging.info(f"Restoring {filename}...")
        response = self._send_request(OP_CODES.RESTORE_FILE, filename)
        if response.status == STATUS_CODES.RESTORE_SUCCESS:
            logging.info(f"{filename} restored successfully!")
            with open(path_to_restore, "wb") as fp:
                fp.write(response.payload)
        else:
            err = RestoreError(response.status, filename)
            logging.warning(str(err))
            raise err

    @on_connect
    def remove_file(self, filename):
        logging.info(f"Removing {filename}...")
        response = self._send_request(OP_CODES.REMOVE_FILE, filename)
        if response.status == STATUS_CODES.BACKUP_OR_REMOVE_SUCCESS:
            logging.info(f"{filename} removed successfully!")
        else:
            err = RemoveError(response.status, filename)
            logging.warning(str(err))
            raise err

    @on_connect
    def list_files(self):
        logging.info(f"Asking for files list...")
        response = self._send_request(OP_CODES.LIST_FILES)
        if response.status == STATUS_CODES.NO_FILES:
            err = ListFilesError(response.status, self.user_id)
            logging.warning(str(err))
            raise err
        return str(response.payload, encoding="utf-8").strip("\n").split("\n")

    def _send_request(self, op_code, file_path=""):
        request = Request(self.user_id, self.VERSION, op_code, file_path)
        build_request = request.build()
        logging.debug(str({'op_code': op_code, 'file_path': file_path}) +
                      " -> " + 
                      str(request) +
                      " -> " +
                      str(build_request))
        
        logging.debug(f"Sending request {request}...")
        self._socket.send(build_request)

        response = Response()
        response.parse_from_socket(self._socket) 
        logging.debug(f"Got response {response}")
        return response

def get_server_info():
    with open(SERVER_INFO_FILE, "rb") as fp:
        address, port = str(fp.read(), encoding="utf-8").split(":")
        return address, int(port)

def get_random_int():
    return random.randint(0, 4294967295)

def get_files_to_restore():
    with open(BACKUP_FILE, "rb") as fp:
        return str(fp.read(), encoding="utf-8").splitlines()

def main():
    server_address, server_port = get_server_info()
    user_id = get_random_int()
    first_file, second_file = get_files_to_restore()
    client = Client(user_id, server_address, server_port)
    client.backup_file(first_file)
    client.backup_file(second_file)
    print("Client files on server:", client.list_files())
    client.restore_file(first_file, r"tmp")
    client.remove_file(first_file)
    try:
        client.restore_file(first_file, "tmp2")
    except RestoreError as e:
        print("Got an exception:", str(e))
    client.close()
    print("Finito!")

if __name__ == "__main__":
    main()