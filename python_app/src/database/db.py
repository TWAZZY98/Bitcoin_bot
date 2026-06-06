# this is the database connection and set up file

import sqlite3

DB_PATH = "database/market_data.db"


def get_connected():
    con = sqlite3.connect(DB_PATH)
    con.execute("PRAGMA journal_mode=WAL")
    return con

def init_db():
    con = get_connected()
    cur = con.cursor()
    
    cur.execute("""
    CREATE TABLE IF NOT EXISTS btc_candle (
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
    
    cur.execute("""
    CREATE TABLE IF NOT EXISTS btc_ticker (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        symbol TEXT,
        price FLOAT
    )                   
    """)
    
    con.commit()
    con.close()