from binance_client import BinanceClient
import time

B_C = BinanceClient()

previous_btc = None
previous_usdt = None
previous_equity = None

while True:
    account = B_C.get_client()

    btc = next(b for b in account["balances"] if b["asset"] == "BTC")
    usdt = next(b for b in account["balances"] if b["asset"] == "USDT")

    current_btc = float(btc["free"])
    current_usdt = float(usdt["free"])

    btc_price = float(B_C.get_current_price("BTCUSDT"))

    # Total portfolio equity, valued in USDT
    current_equity = current_usdt + (current_btc * btc_price)

    if previous_btc is not None and current_btc != previous_btc:
        direction = "UP" if current_btc > previous_btc else "DOWN"
        print(f"BTC balance: {current_btc:.8f} ({direction})")

    if previous_usdt is not None and current_usdt != previous_usdt:
        direction = "UP" if current_usdt > previous_usdt else "DOWN"
        print(f"USDT balance: {current_usdt:.2f} ({direction})")

    if previous_equity is not None and current_equity != previous_equity:
        direction = "UP" if current_equity > previous_equity else "DOWN"
        change = current_equity - previous_equity
        print(
            f"Total equity: {current_equity:.2f} USDT "
            f"({direction}, {change:+.2f} USDT)"
        )

    previous_btc = current_btc
    previous_usdt = current_usdt
    previous_equity = current_equity

    time.sleep(5)