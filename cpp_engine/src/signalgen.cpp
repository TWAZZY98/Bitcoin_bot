#include "include/signalgen.h"
#include <sstream>
#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <string_view>
#include <iostream>

// CSV layout, one row per second
// 0 symbol
// 1 timestamp
// 2 open
// 3 high
// 4 low
// 5 close
// 6 volume

// Unchanged, kept in case other files use it. The strategy below does not call it
vector<string> splitCSV(const std::string& line)
{
    vector<string> fields;
    stringstream ss(line);
    string field;

    while (getline(ss, field, ',')) {
        fields.push_back(field);
    }

    return fields;
}

namespace
{
    // true = print one line per call to stderr explaining why the strategy acts or waits
    constexpr bool DEBUG_LOG = false;

    // ------------------------------------------------------------------
    // CSV columns
    // ------------------------------------------------------------------
    constexpr size_t COL_OPEN   = 2;
    constexpr size_t COL_HIGH   = 3;
    constexpr size_t COL_LOW    = 4;
    constexpr size_t COL_CLOSE  = 5;
    constexpr size_t COL_VOLUME = 6;

    // ------------------------------------------------------------------
    // Windows, counted in bars (1 bar = 1 second)
    // ------------------------------------------------------------------
    constexpr size_t MIN_BARS  = 90;   // valid bars needed before any signal
    constexpr size_t VOL_WIN   = 60;   // volatility, efficiency ratio, VWAP, volume baseline
    constexpr size_t SHOCK_WIN = 10;   // short volatility window for the spike guard
    constexpr size_t ACT_WIN   = 30;   // activity check
    constexpr size_t MOM_WIN   = 30;   // main momentum horizon
    constexpr size_t FAST_WIN  = 5;    // fast momentum (crash exit) and volume surge
    constexpr size_t FLOW_WIN  = 15;   // order flow proxy
    constexpr int    EMA_FAST  = 8;
    constexpr int    EMA_SLOW  = 34;
    static_assert(MIN_BARS > VOL_WIN + 1, "MIN_BARS must exceed the longest window");
    static_assert(EMA_SLOW < static_cast<int>(MIN_BARS), "MIN_BARS must exceed the slow EMA");

    // ------------------------------------------------------------------
    // Entry filters
    // ------------------------------------------------------------------
    constexpr double SIGMA_MIN       = 1e-7;  // per-second return RMS below this = dead market
    constexpr double SIGMA_MAX       = 2e-3;  // above this = chaotic market
    constexpr double SHOCK_RATIO     = 2.5;   // 10s vol / 60s vol above this = spike, do not enter
    constexpr double MIN_ACTIVE_FRAC = 0.70;  // share of recent bars that must have traded volume
    constexpr double ER_MIN          = 0.28;  // efficiency ratio below this = choppy, no trend to follow
    constexpr double SURGE_MIN       = 1.0;   // last 5s volume vs 60s average, participation check
    constexpr double STRETCH_MAX     = 2.0;   // price this far above the fast EMA = already chased

    // ------------------------------------------------------------------
    // Composite score
    // ------------------------------------------------------------------
    constexpr double W_TREND = 0.30, W_MOM = 0.30, W_FLOW = 0.25, W_VWAP = 0.15;
    constexpr double TREND_NORM = 0.6;   // EMA gap, in noise units, that maps to tanh(1)
    constexpr double MOM_NORM   = 2.0;   // 15s momentum z-score that maps to tanh(1)
    constexpr double VWAP_NORM  = 1.0;   // distance from VWAP, in noise units, that maps to tanh(1)
    constexpr double FLOW_GAIN  = 2.0;   // order flow is in [-1, 1] but rarely beyond +-0.3
    constexpr double ENTRY_SCORE = 0.5; // BUY needs at least this
    constexpr double EXIT_SCORE  = -0.30;// SELL (exit the long) at or below this, wide gap to ENTRY_SCORE on purpose
    constexpr double CRASH_Z     = 3.5;  // 5s drop of this many noise units = immediate exit

    // ------------------------------------------------------------------
    // Cost gate
    // IBKR crypto (Paxos): 0.18% per side at the lowest volume tier = 36 bps round trip,
    // plus spread and slippage. SET THIS TO YOUR REAL ALL-IN ROUND-TRIP COST.
    // ------------------------------------------------------------------
    constexpr double COST_BPS    = 15.0;
    constexpr double COST_COVER  = 1.1;  // expected move must beat cost by this factor
    constexpr double HOLD_BARS   = 600;  // intended holding time in seconds
    constexpr double EDGE_SIGMAS = 2.0;   // a score of 1.0 is assumed to mean a 2-sigma move over HOLD_BARS

    struct Bar { double open, high, low, close, volume; };

    // Allocation-free CSV parse of the 5 numeric fields. Rejects malformed or nonsensical rows.
    bool parseBar(const string& line, Bar& out)
    {
        std::string_view rest(line);
        double v[COL_VOLUME + 1] = {};

        for (size_t f = 0; f <= COL_VOLUME; f++)
        {
            const size_t comma = rest.find(',');
            std::string_view tok = rest.substr(0, comma);

            if (f >= COL_OPEN)
            {
                while (!tok.empty() && tok.front() == ' ') tok.remove_prefix(1);
                while (!tok.empty() && (tok.back() == ' ' || tok.back() == '\r' || tok.back() == '\n'))
                    tok.remove_suffix(1);

                const char* p0 = tok.data();
                const char* p1 = p0 + tok.size();
                const auto res = std::from_chars(p0, p1, v[f]);
                if (res.ec != std::errc() || res.ptr != p1) return false;
            }

            if (comma == std::string_view::npos)
            {
                if (f < COL_VOLUME) return false;   // row has too few fields
                break;
            }
            rest.remove_prefix(comma + 1);
        }

        out.open   = v[COL_OPEN];
        out.high   = v[COL_HIGH];
        out.low    = v[COL_LOW];
        out.close  = v[COL_CLOSE];
        out.volume = v[COL_VOLUME];

        return std::isfinite(out.open) && std::isfinite(out.high) && std::isfinite(out.low) &&
               std::isfinite(out.close) && std::isfinite(out.volume) &&
               out.low > 0 && out.close > 0 && out.high >= out.low && out.volume >= 0;
    }

    // Root mean square of the last w values (zero-mean assumption, standard at 1s)
    double rmsLast(const vector<double>& x, size_t w)
    {
        double s = 0;
        for (size_t i = x.size() - w; i < x.size(); i++) s += x[i] * x[i];
        return std::sqrt(s / w);
    }

    // Kaufman efficiency ratio: net move / total path, 0 = pure chop, 1 = straight line
    double efficiencyRatio(const vector<double>& lr, size_t w)
    {
        double net = 0, path = 0;
        for (size_t i = lr.size() - w; i < lr.size(); i++)
        {
            net  += lr[i];
            path += std::fabs(lr[i]);
        }
        return path > 0 ? std::fabs(net) / path : 0.0;
    }

    // EMA of closes, seeded with the SMA of the first `period` bars
    double emaClose(const vector<Bar>& b, int period)
    {
        const double k = 2.0 / (period + 1);
        double e = 0;
        for (int i = 0; i < period; i++) e += b[i].close;
        e /= period;
        for (size_t i = period; i < b.size(); i++)
            e = b[i].close * k + e * (1.0 - k);
        return e;
    }

    // Volume weighted average of the typical price over the last w bars
    double vwapLast(const vector<Bar>& b, size_t w)
    {
        double pv = 0, vol = 0, tpSum = 0;
        for (size_t i = b.size() - w; i < b.size(); i++)
        {
            const double tp = (b[i].high + b[i].low + b[i].close) / 3.0;
            pv    += tp * b[i].volume;
            vol   += b[i].volume;
            tpSum += tp;
        }
        return vol > 0 ? pv / vol : tpSum / w;
    }

    double meanVolume(const vector<Bar>& b, size_t w)
    {
        double s = 0;
        for (size_t i = b.size() - w; i < b.size(); i++) s += b[i].volume;
        return s / w;
    }

    double activeFraction(const vector<Bar>& b, size_t w)
    {
        size_t c = 0;
        for (size_t i = b.size() - w; i < b.size(); i++)
            if (b[i].volume > 0) c++;
        return static_cast<double>(c) / w;
    }

    // Volume weighted order flow proxy in [-1, 1].
    // Per bar: where the close sits inside the bar range (+1 = on the high, -1 = on the low).
    // Bars with no range (single price) fall back to the tick rule: up / down vs the previous close.
    double flowLast(const vector<Bar>& b, size_t w)
    {
        double num = 0, den = 0;
        for (size_t i = b.size() - w; i < b.size(); i++)
        {
            const double range = b[i].high - b[i].low;
            double dir;
            if (range > 0)
                dir = std::clamp((2.0 * b[i].close - b[i].high - b[i].low) / range, -1.0, 1.0);
            else
            {
                const double d = b[i].close - b[i - 1].close;
                dir = d > 0 ? 1.0 : (d < 0 ? -1.0 : 0.0);
            }
            num += dir * b[i].volume;
            den += b[i].volume;
        }
        return den > 0 ? num / den : 0.0;
    }
}

// Long-only, so SELL means "exit the long", never "open a short".
//
// Layers:
//   1. data hygiene        bad rows dropped, too little history = HOLD
//   2. exits               crash guard and score collapse, never blocked by entry filters
//   3. entry filters       volatility band, spike guard, activity, regime, participation, not-chasing
//   4. conviction          composite score of trend, momentum, order flow and VWAP position
//   5. cost gate           expected move over the holding horizon must beat round-trip costs
Sygnal StansTestStrat::generate(const deque<string>& buffer)
{
    Sygnal ret;
    ret.state = SignalStates::HOLD;
    // TEMPORARY DEBUG TRACE
    std::cerr << "[init check] buffer.size()=" << buffer.size() 
              << " detpth_=\"" << detpth_ << "\"\n";


    // ---- 1. Parse oldest -> newest. Scratch vectors are reused, so no heap allocation per call.
    const size_t depth = static_cast<size_t>(detpth_ > 0 ? detpth_ : 0);
    const size_t n = std::min(buffer.size(), depth);
    if (n < MIN_BARS) return ret;

    thread_local vector<Bar>    bars;
    thread_local vector<double> lr;     // log returns, lr[0] is unused

    bars.clear();
    for (size_t i = buffer.size() - n; i < buffer.size(); i++)
    {
        Bar b;
        if (parseBar(buffer[i], b)) bars.push_back(b);
    }
    if (bars.size() < MIN_BARS) return ret;

    const size_t N    = bars.size();
    const double last = bars[N - 1].close;

    lr.assign(N, 0.0);
    for (size_t i = 1; i < N; i++)
        lr[i] = std::log(bars[i].close / bars[i - 1].close);

    // ---- 2. Volatility: the yardstick everything else is measured in
    const double sigmaRaw   = rmsLast(lr, VOL_WIN);
    const double sigmaShock = rmsLast(lr, SHOCK_WIN);
    const double sigma      = std::max(sigmaRaw, 1e-9);

    // ---- 3. Features, expressed in "noise units" so they adapt to the volatility level
    auto momZ = [&](size_t k) {
        return std::log(last / bars[N - 1 - k].close) / (sigma * std::sqrt(static_cast<double>(k)));
    };
    const double mom15 = momZ(MOM_WIN);
    const double mom5  = momZ(FAST_WIN);

    const double emaF    = emaClose(bars, EMA_FAST);
    const double emaS    = emaClose(bars, EMA_SLOW);
    const double trendZ  = std::log(emaF / emaS) / (sigma * std::sqrt(static_cast<double>(EMA_SLOW)));
    const double stretch = std::log(last / emaF) / (sigma * std::sqrt(static_cast<double>(EMA_FAST)));

    const double vwap  = vwapLast(bars, VOL_WIN);
    const double vwapZ = std::log(last / vwap) / (sigma * std::sqrt(static_cast<double>(VOL_WIN)));
    const double flow  = flowLast(bars, FLOW_WIN);

    // ---- 4. Composite score in [-1, 1]
    const double score = W_TREND * std::tanh(trendZ / TREND_NORM)
                       + W_MOM   * std::tanh(mom15  / MOM_NORM)
                       + W_FLOW  * std::tanh(flow   * FLOW_GAIN)
                       + W_VWAP  * std::tanh(vwapZ  / VWAP_NORM);

    // ---- 5. Diagnostics helper. Set DEBUG_LOG = true to see why the strategy acts or waits.
    const double expectedMoveBps = score * EDGE_SIGMAS * sigma * std::sqrt(HOLD_BARS) * 1e4;
    auto decide = [&](SignalStates st, const char* why)
    {
        ret.state = st;
        if (DEBUG_LOG)
        {
            std::cerr << "[strat] "
                      << (st == SignalStates::BUY ? "BUY " : st == SignalStates::SELL ? "SELL" : "HOLD")
                      << " " << why
                      << " | score=" << score << " sigma=" << sigmaRaw * 1e4 << "bp"
                      << " mom15=" << mom15 << " mom5=" << mom5 << " trendZ=" << trendZ
                      << " flow=" << flow << " move=" << expectedMoveBps
                      << "bp need=" << COST_BPS * COST_COVER << "bp\n";
        }
        return ret;
    };

    // ---- 6. Exits first. They are never blocked by the entry filters.
    if (mom5 <= -CRASH_Z)    return decide(SignalStates::SELL, "crash guard");
    if (score <= EXIT_SCORE) return decide(SignalStates::SELL, "score collapsed");

    // ---- 7. Entry filters (any failure = HOLD)
    if (sigmaRaw < SIGMA_MIN || sigmaRaw > SIGMA_MAX)
        return decide(SignalStates::HOLD, "volatility outside band");
    if (sigmaShock > SHOCK_RATIO * sigma)
        return decide(SignalStates::HOLD, "volatility spike");
    if (activeFraction(bars, ACT_WIN) < MIN_ACTIVE_FRAC)
        return decide(SignalStates::HOLD, "thin or frozen feed");
    if (efficiencyRatio(lr, VOL_WIN) < ER_MIN)
        return decide(SignalStates::HOLD, "choppy, no trend to follow");

    const double volBase = meanVolume(bars, VOL_WIN);
    if (volBase <= 0 || meanVolume(bars, FAST_WIN) < SURGE_MIN * volBase)
        return decide(SignalStates::HOLD, "no volume participation");

    if (stretch > STRETCH_MAX)
        return decide(SignalStates::HOLD, "already stretched, not chasing");
    if (score < ENTRY_SCORE)
        return decide(SignalStates::HOLD, "score below entry threshold");

    // ---- 8. Cost gate: expected move over the holding horizon must cover the round trip
    if (expectedMoveBps < COST_BPS * COST_COVER)
        return decide(SignalStates::HOLD, "blocked by cost gate");

    return decide(SignalStates::BUY, "all filters passed");
}