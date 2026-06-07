import json
import websocket
import pandas as pd
from database import db

class Datastream:
    def __init__(self):
        assets = db.get_assets()

        streams = [
            f"{asset.lower()}@kline_1m"
            for asset in assets
        ]

        stream_string = "/".join(streams)

        self.socket = (
            "wss://stream.binance.com:9443/stream?streams="
            + stream_string
        )

        self.ws = websocket.WebSocketApp(
            self.socket,
            on_message=self.on_message
        )

    def on_message(self, ws, message):
        data = json.loads(message)

        print(data)

    def run(self):
        self.ws.run_forever()
        
    def get_parsed_assets(self):
        return self.assets
    
    def get_paresed_socket(self):
        return self.socket
    

