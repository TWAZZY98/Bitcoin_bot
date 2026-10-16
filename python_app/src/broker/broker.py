import socket
from binance_client import BinanceClient

def recieve_signal():
    print("[BROKER] Function entered", flush=True)

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        print("[BROKER] Socket created", flush=True)

        sock.bind(("0.0.0.0", 5051))
        print("[BROKER] Bound to port 5051", flush=True)

        c = BinanceClient()
        print("[BROKER] BinanceClient initialized", flush=True)

        while True:
            print("[BROKER] Waiting for signal...", flush=True)

            data, addr = sock.recvfrom(1024)

            print(f"[BROKER] Received {data!r} from {addr}", flush=True)

            message = data.decode("utf-8")
            signal, symbol, quantity = message.split(",")

            if signal == "BUY":
                c.buy_coin(symbol, quantity)
            elif signal == "SELL":
                c.sell_coin(symbol, quantity)

    except Exception:
        traceback.print_exc()