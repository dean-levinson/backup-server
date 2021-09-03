import os
import socket
import logging
import struct
from typing import Sized
from functools import wraps

logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")

STATUS_CODES = {
    210: "",
    211: "",
    1001: ""
}

class OP_CODES(object):
    BACKUP_FILE = 100
    RESTORE_FILE = 200
    REMOVE_FILE = 201
    LIST_FILES = 202


class Request(object):
    def __init__(self, user_id, version, op_code, filepath):
        self.user_id = user_id
        self.version = version
        self.op_code = op_code

        if filepath:
            self.filename = bytes(os.path.basename(filepath), encoding="utf-8")
            self.name_len = len(self.filename)
        else:
            self.filename = None
            self.name_len = None

        if not os.path.isfile(filepath):
            self.payload = None
            self.size = None
        else:
            with open(filepath, "rb") as fp:
                self.payload = fp.read()
            self.size = len(self.payload)
    
    def build(self):
        if self.payload:
            packed = struct.pack(f'<IBBH{self.name_len}sI{self.size}s', self.user_id,
                                                                        self.version,
                                                                        self.op_code,
                                                                        self.name_len,
                                                                        self.filename,
                                                                        self.size,
                                                                        self.payload)
        elif not self.filename:
            packed = struct.pack(f'<IBBH{self.name_len}s', self.user_id,
                                                           self.version,
                                                           self.op_code,
                                                           self.name_len,
                                                           self.filename)
        
        else:
            packed = struct.pack(f'<IBB', self.user_id,
                                         self.version,
                                         self.op_code)
                                                          
        return packed

    def __str__(self):
        if self.filename:
            return f"<Request user_id={self.user_id}, version={self.version}, opcode={self.op_code}, filename={self.filename}>"
        else:
            return f"<Request user_id={self.user_id}, version={self.version}, opcode={self.op_code}>"

class ClientNotConnect(Exception):
    pass
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
        logging.info(f"Conencting to ({self.server_ip},{self.server_port})...")
        self._socket.connect((self.server_ip, self.server_port))
        logging.info("Connected successfully!")
    
    def close(self):
        self._socket.close()

    def backup_file(self, file_path):
        logging.info(f"Backing up '{file_path}'")
        self._send_request(OP_CODES.BACKUP_FILE, file_path)
    
    def on_connect(inner):
        @wraps(inner)
        def wrapper(self, *args, **kwargs):
            if not self._socket:
                raise ClientNotConnect()
            return inner(self, *args, **kwargs)
        return wrapper

    @on_connect
    def _send_request(self, op_code, file_path=""):
        request = Request(self.user_id, self.VERSION, op_code, file_path)
        build_request = request.build()
        logging.debug(str({'op_code': op_code, 'file_path': file_path}) +
                      " -> " + 
                      str(request) +
                      " -> " +
                      str(build_request))
        
        logging.info(f"Sending {request}")
        self._socket.send(build_request)	

def main():
    # Todo: read parameters from file
    client = Client(1234, "127.0.0.1", 5441)
    client.connect()
    client.backup_file(r"D:\tmp\2021-09-03\test.txt")
    

if __name__ == "__main__":
    main()