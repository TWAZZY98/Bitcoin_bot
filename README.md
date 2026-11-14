# Bitcoin_bot

A Bitcoin trading bot with a high-performance **C++23** engine for market data and signal generation, and a **Python 3.12** broker that executes trades on Binance and tracks open and closed positions.

![Status](https://img.shields.io/badge/status-in%20development-orange)
![C++](https://img.shields.io/badge/C%2B%2B-23-blue)
![Python](https://img.shields.io/badge/Python-3.12-blue)
![License](https://img.shields.io/badge/license-Apache--2.0-green)

> **Disclaimer:** This project is for educational and research purposes. Trading cryptocurrency carries real financial risk. Test with paper trading before using real funds. You are responsible for any losses.

---

## Screenshots

<!-- Put your images in docs/images/ and update the file names if needed -->

| Dashboard | Live signals |
| --- | --- |
| ![Dashboard](docs/images/dashboard.png) | ![Signals](docs/images/signals.png) |

| Backtest results | Architecture |
| --- | --- |
| ![Backtest](docs/images/backtest.png) | ![Architecture](docs/images/architecture.png) |

---

## Features

- Real-time price ingestion into a ring buffer (`PriceBuffer`)
- SQLite persistence for candles and trades (`DbHandler`)
- Strategy engine with pluggable signal generation (`signalgen`)
- Paper trading on the Binance test network for strategy testing
- Python broker that executes orders on Binance and tracks positions
- Simple Python CLI monitoring app

---

## Project Structure

```
Bitcoin_bot/
├── cpp_engine/     # C++23 engine: data client, strategy, persistence
├── python_app/     # Python 3.12 broker and CLI monitor
├── docs/           # Documentation and images
├── LICENSE         # Apache-2.0
└── README.md
```

---

## Requirements

| Component | Version |
| --- | --- |
| C++ compiler | GCC 13+ or Clang 17+ with C++23 support |
| CMake | 3.20+ |
| SQLite3 | dev libraries (`libsqlite3-dev`) |
| Python | 3.12 |
| pip | latest |
| Binance account | live API keys for real trading |
| Binance testnet account | API keys for paper trading |

---

## Setup

### 1. Clone the repository

```bash
git clone https://github.com/TWAZZY98/Bitcoin_bot.git
cd Bitcoin_bot
```

### 2. Build the C++ engine

Install dependencies (Ubuntu / WSL):

```bash
sudo apt update
sudo apt install build-essential cmake libsqlite3-dev
```

Build with CMake:

```bash
cd cpp_engine
cmake -S . -B build
cmake --build build
```

Or compile by hand with g++:

```bash
cd cpp_engine/src
g++ main.cpp client.cpp dbhandler.cpp pricebuffer.cpp signalgen.cpp -std=c++23 -pthread -lsqlite3 -o btc_engine
```

### 3. Set up the Python app

```bash
cd python_app
python3.12 -m venv venv
source venv/bin/activate        # Windows: venv\Scripts\activate
pip install -r requirements.txt
```

### 4. Configuration

Create a `.env` file inside the `python_app` folder:

```env
BINANCE_BASE_URL="https://api.binance.com"

# Live trading keys (linked to your real account)
BINANCE_API_KEY=
BINANCE_API_SECRET=

# Paper trading keys (from the Binance test network)
BINANCE_API_KEY_P=
BINANCE_API_KEY_SECRET_P=
```

> Never commit API keys. Make sure `.env` is listed in `.gitignore`.

---

## Usage

Start the Python broker first, then the C++ engine.

**Terminal 1: Python broker**

```bash
cd python_app
source venv/bin/activate
python main.py
```

**Terminal 2: C++ engine**

If you built with CMake:

```bash
./cpp_engine/build/bitcoin_bot
```

If you built by hand:

```bash
./cpp_engine/src/btc_engine
```

---

## How It Works

1. The engine receives price data and stores it in the `PriceBuffer`.
2. Candles are persisted to SQLite through the `DbHandler`.
3. The strategy reads the buffer, generates buy, sell or hold signals and sends them to the Python broker.
4. The Python broker executes trades on Binance and tracks opening and closing positions.

---

## Roadmap

- [x] Paper trading mode
- [ ] Live order execution
- [ ] More strategies and indicators
- [ ] Backtesting reports
- [ ] Unit tests and CI

---

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

---

## License

Distributed under the Apache-2.0 License. See [LICENSE](LICENSE) for details.