#insert and query functions for the database

from database import db

def insert_ticker(symbol:str, price:float):
    con = db.get_connected()
    cur = con.cursor()
    try:
        print("[INFO] Ticker insert initiated")
        cur.execute(f"""
            INSERT INTO {symbol}_ticker (
                symbol, price
                
            ) VALUES (?, ?)
        """,(symbol, price))
        con.commit()
    except Exception as e:
        print(f"[ERROR] {type(e).__name__}: {e}")
    finally:
        con.close()

def insert_candle(input:dict):
    con = db.get_connected()
    cur = con.cursor()
    try:
        print("[INFO] Initiated candle insert")
        cur.execute(f"""
            INSERT INTO {input['symbol']}_candle (
                symbol, open_time, open, high, low, close, volume
            ) VALUES (?,?,?,?,?,?,?)
        """,(
            input['symbol'],
            input['open_time'],
            input['open'],
            input['high'],
            input['low'],
            input['close'],
            input['volume']
        ))
        
        print(f"[INFO] ROWS inserted: {cur.rowcount}")
        print(f"[INFO] Last row ID: {cur.lastrowid}")
        con.commit()
    except Exception as e:
        print(f"[ERROR] {type(e).__name__}: {e}")
    finally:
        con.close()