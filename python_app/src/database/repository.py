#insert and query functions for the database

from database import db

def insert_ticker(symbol:str, price:float):
    con = db.get_connected()
    cur = con.cursor()
    
    cur.execute("""
        INSERT INTO btc_ticker (
            symbol, price
            
        ) VALUES (?, ?)
    """,(symbol, price))
    con.commit()
    con.close()