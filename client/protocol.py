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
    def __init__(self, binary_response):
        self.version = None
        self.status = None
        self.name_len = None
        self.filename = None
        self.size = None
        self.payload = None

        logging.debug(f"Got response: {binary_response}")
        self.parse_response(binary_response)

    def parse_response(self, binary_response):
        self.version, self.status = struct.unpack('<BH', binary_response[:3])
        binary_response = binary_response[3:]
         
        if self.status in (STATUS_CODES.RESTORE_SUCCESS, STATUS_CODES.BACKUP_OR_REMOVE_SUCCESS,
                           STATUS_CODES.LIST_FILES_SUCCESS, STATUS_CODES.FILE_NOT_FOUND):
            self.name_len = struct.unpack('<H', binary_response[:2])[0]
            binary_response = binary_response[2:]
            self.filename = struct.unpack(f'<{self.name_len}s', binary_response[:self.name_len])[0]
            binary_response = binary_response[self.name_len:]

        if self.status in (STATUS_CODES.RESTORE_SUCCESS, STATUS_CODES.LIST_FILES_SUCCESS):
            self.size = struct.unpack('<I', binary_response[:4])[0]
            binary_response = binary_response[4:]
            self.payload = struct.unpack(f'<{self.size}s', binary_response[:self.size])[0]
            binary_response = binary_response[self.size:]

    def __str__(self):
        if self.filename:
            return f"<Request version={self.version}, status={self.status}, filename={self.filename}>"
        else:
            return f"<Request version={self.version}, status={self.status}>"