from binance.client import Client
import os

client = Client(
    os.getenv("BINANCE_API_KEY"),
    os.getenv("BINANCE_API_SECRET")
)

price  = client.get_symbol_ticker(symbol="BTCUSDT")
print(price)