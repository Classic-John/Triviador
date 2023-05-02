import sqlite3
import jsonManipulator
import sys


def insert_choice(connection, **kwargs):
    cursor = connection.cursor()
    sqlite_insert_with_param = """INSERT INTO choiceQuestion
                            (difficulty, statement, choices, correctIdx) 
                            VALUES 
                            (?, ?, ?, ?)"""

    data_tuple = (kwargs['difficulty'], kwargs['statement'], kwargs['answers'], kwargs['correct'])
    cursor.execute(sqlite_insert_with_param, data_tuple)
    sqliteConnection.commit()
    print("Record inserted successfully into choiceQuestion table ", cursor.rowcount)
    cursor.close()

data = jsonManipulator.get_questions(sys.argv[1])
try:
    sqliteConnection = sqlite3.connect(r"C:\Users\balin\Documents\Universitate\Year_2\Modern_C++\Final_Project_Repo\ProjectModernCpp\src\triviador.db")
    print("Successfully Connected to SQLite")
    for question in data:
        insert_choice(sqliteConnection, **question)
except sqlite3.Error as error:
        print("Failed to insert data into sqlite table", error)
finally:
    if sqliteConnection:
        sqliteConnection.close()
        print("The SQLite connection is closed")