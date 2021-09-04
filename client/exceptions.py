class BackupError(Exception):
    def __init__(self, status, filepath):
        super(self).__init__(f"Failed backuping '{filepath}', got status {status}")

class RestoreError(Exception):
    def __init__(self, status, filename):
        super(self).__init__(f"Failed Restoring '{filename}', got status {status}")

class RemoveError(Exception):
    def __init__(self, status, filename):
        super(self).__init__(f"Failed removing '{filename}', got status {status}")
