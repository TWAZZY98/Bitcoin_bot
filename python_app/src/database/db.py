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
    try:
        print(f"[Info] Initiated {name}_candle table creation")
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
        
    except Exception as e:
        print(f"[ERROR] {type(e).__name__}: {e}")
    finally:
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
    
# list of assests that have a initiated table
# move to a json file later of use some way to make it easy to add or delete assets
def get_assets() -> list:
    return ["BTCUSDT"]

def init_db():
    assets = get_assets()
    for a in assets:
        create_ticker_table(a)
        create_candle_table(a)