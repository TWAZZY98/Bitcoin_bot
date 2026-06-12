from binance.client import Client
import os
from database import db, repository
from stream import datastream



def main():
    db.init_db()
    
    client = Client(
        os.getenv("BINANCE_API_KEY"),
        os.getenv("BINANCE_API_SECRET")
    )

    price  = client.get_symbol_ticker(symbol="BTCUSDT")
    print(price)
    repository.insert_ticker(price['symbol'],price['price'])
    d = datastream.Datastream()
    d.run()

if __name__ == "__main__":
    main()