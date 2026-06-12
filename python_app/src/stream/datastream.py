import json
import websocket
from database import db, repository
from stream import messagehandler as mh

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
        #enters data into the message handler into the queue
        parsed_data = self.massageHandler.parse(data)
        #candle_queue.put(parsed_data)
        #implement lock free implementation with a queue later
        repository.insert_candle(parsed_data)


    def run(self):
        self.ws.run_forever()
        
    def get_parsed_assets(self):
        return self.assets
    
    def get_paresed_socket(self):
        return self.socket
    

