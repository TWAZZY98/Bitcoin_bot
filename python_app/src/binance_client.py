import os
import time
from dotenv import load_dotenv
from binance.client import Client

load_dotenv()

class BinanceClient:

    def __init__(self):

        self.client = Client(os.getenv("BINANCE_API_KEY_P"),os.getenv("BINANCE_API_KEY_SECRET_P"),testnet= True)

        
    def get_client(self):
        print(self.client.get_account())
        
    def get_current_price(self, symbol):
        ticker = self.client.get_symbol_ticker(symbol= symbol)
        return float(ticker['price'])
        
    def buy_coin(self,symbol, amount):
        order = self.client.order_market_buy(symbol= symbol,quantity = amount)
        print(f"Buy order done: {order}")
        
    def sell_coin(self,symbol, amount):
        order = self.client.order_market_sell(symbol= symbol,quantity = amount)
        print(f"Sell order done: {order}")