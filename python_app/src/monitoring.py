import time
from binance_client import BinanceClient

B_C = BinanceClient()


def get_total(account, asset):
    """free + locked, 0.0 if the asset isn't listed."""
    for b in account["balances"]:
        if b["asset"] == asset:
            return float(b["free"]) + float(b["locked"])
    return 0.0


prev = None  # (btc, usdt, equity)

while True:
    try:
        account = B_C.get_client()
        price = float(B_C.get_current_price("BTCUSDT"))
    except Exception as e:
        print(f"[WARN] poll failed: {e}")
        time.sleep(5)
        continue

    btc = get_total(account, "BTC")
    usdt = get_total(account, "USDT")
    equity = usdt + btc * price

    if prev is None:
        print(f"Start: {btc:.8f} BTC | {usdt:.2f} USDT | equity {equity:.2f} USDT")
    else:
        p_btc, p_usdt, p_equity = prev
        # Only report when balances change (a trade happened), not on price drift
        if abs(btc - p_btc) > 1e-8 or abs(usdt - p_usdt) > 0.005:
            if abs(btc - p_btc) > 1e-8:
                print(f"BTC:  {btc:.8f} ({'UP' if btc > p_btc else 'DOWN'})")
            if abs(usdt - p_usdt) > 0.005:
                print(f"USDT: {usdt:.2f} ({'UP' if usdt > p_usdt else 'DOWN'})")
            change = equity - p_equity
            print(f"Equity: {equity:.2f} USDT ({change:+.2f})")

    prev = (btc, usdt, equity)
    time.sleep(2)