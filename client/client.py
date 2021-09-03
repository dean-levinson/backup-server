import socket
import logging
from protocol import Request, Response, OP_CODES, STATUS_CODES
from functools import wraps

logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")


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
        response = self._send_request(OP_CODES.BACKUP_FILE, file_path)
        return response
    
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

        # todo - receive fixed size according to the request.
        return Response(self._socket.recv(1024)) # Todo - parse response

def main():
    # Todo: read parameters from file
    client = Client(1234, "127.0.0.1", 5441)
    client.connect()
    response = client.backup_file(r"D:\tmp\2021-09-03\test.txt")
    logging.debug("response: " + str(response))

if __name__ == "__main__":
    main()