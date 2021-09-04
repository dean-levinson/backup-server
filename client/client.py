import socket
import logging
from protocol import Request, Response, OP_CODES, STATUS_CODES
from functools import wraps

logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")


class ClientNotConnect(Exception):
    pass

class BackupError(Exception):
    def __init__(self, status, filepath):
        super(self).__init__(f"Failed backuping '{filepath}', got status {status}")

class RestoreError(Exception):
    def __init__(self, status, filename):
        super(self).__init__(f"Failed Restoring '{filename}', got status {status}")

class RemoveError(Exception):
    def __init__(self, status, filename):
        super(self).__init__(f"Failed removing '{filename}', got status {status}")

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

    def connect(self):
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        logging.debug(f"Conencting to ({self.server_ip},{self.server_port})")
        self._socket.connect((self.server_ip, self.server_port))
        logging.debug("Connected successfully!")
        return socket
    
    def is_connected(self):
        return bool(self._socket)

    def close(self):
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
        logging.info(f"Backing up '{file_path}'")
        response = self._send_request(OP_CODES.BACKUP_FILE, file_path)
        if response.status == STATUS_CODES.BACKUP_OR_REMOVE_SUCCESS:
            logging.info(f"{file_path} backed up successfully!")
        else:
            err = BackupError(response.status, file_path)
            logging.warning(str(err))
            raise err

    @on_connect
    def restore_file(self, filename, path_to_restore):
        logging.info(f"Restoring {filename}")
        response = self._send_request(OP_CODES.RESTORE_FILE, filename)
        if response.status == STATUS_CODES.RESTORE_SUCCESS:
            logging.info(f"{filename} restored successfully!")
            with open(path_to_restore, "wb") as fp:
                fp.write(path_to_restore)
        else:
            err = RestoreError(response.status, filename)
            logging.warning(str(err))
            raise err

    @on_connect
    def restore_file(self, filename):
        logging.info(f"Removing {filename}")
        response = self._send_request(OP_CODES.REMOVE_FILE, filename)
        if response.status == STATUS_CODES.BACKUP_OR_REMOVE_SUCCESS:
            logging.info(f"{filename} removed successfully!")

        else:
            err = RemoveError(response.status, filename)
            logging.warning(str(err))
            raise err

    @on_connect
    def list_files(self):
        logging.info(f"Asking for files list")
        response = self._send_request(OP_CODES.LIST_FILES)
        return str(response.payload, encoding="utf-8").split("\n")

    def _send_request(self, op_code, file_path=""):
        request = Request(self.user_id, self.VERSION, op_code, file_path)
        build_request = request.build()
        logging.debug(str({'op_code': op_code, 'file_path': file_path}) +
                      " -> " + 
                      str(request) +
                      " -> " +
                      str(build_request))
        
        logging.debug(f"Sending {request}")
        self._socket.send(build_request)

        # todo - receive fixed size according to the request.
        response = Response(self._socket.recv(1024)) # Todo - parse response
        logging.debug(f"Got response {response}")
        return response

def main():
    # Todo: read parameters from file
    client = Client(1234, "127.0.0.1", 5441)
    client.backup_file(r"D:\tmp\2021-09-03\test.txt")
    print(client.list_files())
    

if __name__ == "__main__":
    main()