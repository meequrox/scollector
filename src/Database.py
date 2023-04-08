import sqlite3

class Database:
    """
    Custom wrapper around SQLite3 interface. Supports INSERT, BEGIN, COMMIT operations.
    Implements a method to determine if id exists in database.

    Can be reopened after using close().
    """

    def __init__(self, path : str):
        # Any operations must be prevented if this variable is False
        self.__opened : bool = False

        try:
            self.__handler = sqlite3.connect(path)
            self.__cursor = self.__handler.cursor()
            self.__opened = True
        except sqlite3.Error as error:
            print(f"{self.__class__.__name__}: {error}")

        if self.__opened:
            # Try to create `songs` table, do nothing if it exists
            try:
                self.__cursor.execute("CREATE TABLE songs(id INT NOT NULL UNIQUE,PRIMARY KEY(id));")
            except sqlite3.OperationalError:
                pass

    def is_open(self) -> bool:
        return self.__opened

    def transaction_begin(self):
        """Begin transaction in opened database"""

        if self.__opened:
            try:
                self.__cursor.execute("BEGIN TRANSACTION;")
            except sqlite3.OperationalError as error:
                print(f"{self.__class__.__name__}: {error}")

    def transaction_end(self):
        """End (commit) transaction in opened database"""

        if self.__opened:
            try:
                self.__cursor.execute("END TRANSACTION;")
            except sqlite3.OperationalError as error:
                print(f"{self.__class__.__name__}: {error}")

    def insert(self, _id : int):
        """Insert id into opened database"""

        if self.__opened:
            try:
                self.__cursor.execute(f"INSERT INTO songs(id) VALUES({_id});")
            except sqlite3.OperationalError as error:
                print(f"{self.__class__.__name__}: {error}")

    def id_exists(self, _id : int) -> bool:
        """Check if song id exists in opened database"""

        # 0 if id is not in db
        # 1 if id is in db
        count = 0

        if self.__opened:
            try:
                self.__cursor.execute(f"SELECT 1 FROM songs WHERE id={_id}")
                count = len(self.__cursor.fetchall())
            except sqlite3.OperationalError as error:
                print(f"{self.__class__.__name__}: {error}")

        return bool(count)

    def close(self):
        """Close current connection to database"""

        if self.__opened:
            self.__cursor.close()
            self.__handler.close()
            self.__opened = False
