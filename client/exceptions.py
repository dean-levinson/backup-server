from protocol import STATUS_CODES

class BackupError(Exception):
    def __init__(self, status, filepath):
        super(BackupError, self).__init__(f"Failed backuping '{filepath}', got status {status}")

class RestoreError(Exception):
    def __init__(self, status, filename):
        if status == STATUS_CODES.FILE_NOT_FOUND:
            super(RestoreError, self).__init__(f"File '{filename}' not found on server")
        else:
            super(RestoreError, self).__init__(f"Failed Restoring '{filename}', got status {status}")

class RemoveError(Exception):
    def __init__(self, status, filename):
        super(RemoveError, self).__init__(f"Failed removing '{filename}', got status {status}")

class ListFilesError(Exception):
    def __init__(self, status, user_id):
        if status == STATUS_CODES.NO_FILES:
            super(ListFilesError, self).__init__(f"'User {user_id}' doesn't have files on server")
        else:
            super(ListFilesError, self).__init__(f"Failed list files for user '{user_id}', got status {status}")