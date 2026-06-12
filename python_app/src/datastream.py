import json
import websocket
from database import db, repository

import messagehandler as mh

class Datastream:
    def __init__(self):
        self.massageHandler = mh.Parser()
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
        parsed_data = self.massageHandler.parse(data)
        repository.insert_candle(parsed_data)


    def run(self):
        self.ws.run_forever()
        
    def get_parsed_assets(self):
        return self.assets
    
    def get_paresed_socket(self):
        return self.socket
    

