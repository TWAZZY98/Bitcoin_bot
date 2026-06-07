import pandas as pd

class Parser:
    
    def __init__(self):
        self.numOfParsed =0
    
    def parse(self,input:dict) -> dict:
        symbol = input['data']['s']
        open_time = pd.to_datetime(input['data']['E'],unit='ms')
        open = input['data']['k']['o']
        high = input['data']['k']['h']
        low = input['data']['k']['l']
        close = input['data']['k']['c']
        volume = input['data']['k']['v']
        
        ret = {'symbol':symbol,
               'open_time':open_time,
               'open':open,
               'high':high,
               'low':low,
               'close':close,
               'volume':volume
            }
        self.numOfParsed = self.numOfParsed +1
        print(self.numOfParsed)
        return ret