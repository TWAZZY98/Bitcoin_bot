#insert and query functions for the database

from database import db

def insert_ticker(symbol:str, price:float):
    con = db.get_connected()
    cur = con.cursor()
    
    cur.execute(f"""
        INSERT INTO {symbol}_ticker (
            symbol, price
            
        ) VALUES (?, ?)
    """,(symbol, price))
    con.commit()
    con.close()

def insert_candle(input:dict):
    con = db.get_connected()
    cur = con.cursor()
    print("[INFO Inserted candle]")
    cur.execute(f"""
        INSERT INTO {input['symbol']}_candle (
            symbol, open_time, open, high, low, close, volume
        )    
    """,(input['symbol'],
         input['open_time'],
         input['open'],
         input['high'],
         input['low'],
         input['close'],
         input['volume']))