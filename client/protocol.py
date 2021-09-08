import os
import logging
import struct

class STATUS_CODES(object):
    RESTORE_SUCCESS = 210
    LIST_FILES_SUCCESS = 211
    BACKUP_OR_REMOVE_SUCCESS = 212
    FILE_NOT_FOUND = 1001
    NO_FILES = 1002
    GENERAL_FAILURE = 1003

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
        elif self.filename:
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

class Response(object):
    VERSION_FIELD_SIZE = 1
    STATUS_FIELD_SIZE = 2
    NAME_LEN_FIELD_SIZE = 2
    SIZE_FIELD_SIZE = 4
    
    def __init__(self):
        self.version = None
        self.status = None
        self.name_len = None
        self.filename = None
        self.size = None
        self.payload = None

    def parse_from_socket(self, sock):
        logging.debug("Parsing response from socket...")
        self.parse_headers(sock.recv(self.VERSION_FIELD_SIZE + self.STATUS_FIELD_SIZE))
        logging.debug(f"version - {self.version}, Status - {self.status}")

        if self.has_filename():
            self.parse_name_len(sock.recv(self.NAME_LEN_FIELD_SIZE))
            self.parse_filename(sock.recv(self.name_len))
            logging.debug(f"name_len - {self.name_len}, filename - {self.filename}")

        if self.has_payload():
            self.parse_size(sock.recv(self.SIZE_FIELD_SIZE))
            self.parse_payload(sock.recv(self.size))
            logging.debug(f"payload_size - {self.size}")
        
        logging.debug("Response has completely parsed")

    def has_filename(self):
        return self.status in (STATUS_CODES.RESTORE_SUCCESS, STATUS_CODES.BACKUP_OR_REMOVE_SUCCESS,
                               STATUS_CODES.LIST_FILES_SUCCESS, STATUS_CODES.FILE_NOT_FOUND)
    
    def has_payload(self):
        return self.status in (STATUS_CODES.RESTORE_SUCCESS, STATUS_CODES.LIST_FILES_SUCCESS)

    def parse_headers(self, binary_response):
        self.version, self.status = struct.unpack('<BH', binary_response[:(self.VERSION_FIELD_SIZE +
                                                                           self.STATUS_FIELD_SIZE)])

    def parse_name_len(self, binary_response):         
        self.name_len = struct.unpack('<H', binary_response[:self.NAME_LEN_FIELD_SIZE])[0]
    
    def parse_filename(self, binary_response):
        self.filename = struct.unpack(f'<{self.name_len}s', binary_response[:self.name_len])[0]

    def parse_size(self, binary_response):
        self.size = struct.unpack('<I', binary_response[:self.SIZE_FIELD_SIZE])[0]
    
    def parse_payload(self, binary_response):
        self.payload = struct.unpack(f'<{self.size}s', binary_response[:self.size])[0]

    def __str__(self):
        if self.has_filename():
            return f"<Request version={self.version}, status={self.status}, filename={self.filename}>"
        else:
            return f"<Request version={self.version}, status={self.status}>"