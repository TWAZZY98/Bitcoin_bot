#ifndef SIGNAL_GENERATOR
#define SIGNAL_GENERATOR

#include <vector>
#include <string>
#include <numeric>
#include <sstream>
#include <iostream>
#include <deque>

using namespace std;

enum class SignalStates{
    BUY,
    SELL,
    HOLD
};

struct Sygnal{
    SignalStates state;
    string ticker;
    float price;
    int64_t timestamp;
    string debug;
};

class SignalGenerator{
    public:
    SignalGenerator() = default;
    virtual ~SignalGenerator() = default;
    virtual Sygnal generate(const deque<string>& buffer) = 0;

};

class StansTestStrat : public SignalGenerator {
    public:
    StansTestStrat(int n = 3):detpth_(n){};

    Sygnal generate(const deque<string>& buffer);

    private:
    int detpth_;

};
#endif