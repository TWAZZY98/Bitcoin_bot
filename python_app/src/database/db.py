# this is the database connection and set up file

import sqlite3

DB_PATH = "database/market_data.db"


def get_connected():
    con = sqlite3.connect(DB_PATH)
    con.execute("PRAGMA journal_mode=WAL")
    return con

# This function creates table to store candle data with a chosen name
# doesnt do anything if the table already exists
def create_candle_table(name:str):
    con = get_connected()
    cur = con.cursor()
    
    cur.execute(f"""
    CREATE TABLE IF NOT EXISTS {name}_candle (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        symbol TEXT,
        open_time INTEGER,
        open REAL,
        high REAL,
        low REAL,
        close REAL,
        volume REAL
    )                   
    """)
    
    con.commit()
    con.close()
# creates a table for a ticker with a chosen name
# doesnt do anything if table already exists
def create_ticker_table(name:str):
    con = get_connected()
    cur = con.cursor()

    cur.execute(f"""
    CREATE TABLE IF NOT EXISTS {name}_ticker (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        symbol TEXT,
        price FLOAT
    )                   
    """)
    
    con.commit()
    con.close()

def init_db():
    symbols = ["BTCUSD"]
    for a in symbols:
        create_ticker_table(a)