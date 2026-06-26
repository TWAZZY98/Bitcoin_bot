from binance.client import Client
import os
from database import db, repository
from stream import datastream, dataserver

import threading


def run_stream():
    d = datastream.Datastream()
    d.run()
    
def run_server():
    dataserver.start()

def main():
    db.init_db()
    
    client = Client(
        os.getenv("BINANCE_API_KEY"),
        os.getenv("BINANCE_API_SECRET")
    )

    price  = client.get_symbol_ticker(symbol="BTCUSDT")
    print(price)
    repository.insert_ticker(price['symbol'],price['price'])
    t1 = threading.Thread(target=run_stream,args=())
    t2 = threading.Thread(target= run_server, args=())
    t2.start()
    t1.start()
    

if __name__ == "__main__":
    main()