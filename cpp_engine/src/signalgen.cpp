#include "include/signalgen.h"

// 0 symbol
// 1 timestamp
// 2 open
// 3 high
// 4 low
// 5 close
// 6 volume

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

Sygnal StansTestStrat::generate(const deque<string>& buffer)
{

    vector<float> close_data;

    int n = min((int)buffer.size(), detpth_);
    float average{0};

    for (int i = 0; i < n; i++)
    {
        const string& candle = buffer[buffer.size() - i - 1];

        vector<string> a = splitCSV(candle);

        if (a.size() >= 6)
        {
            float close = stof(a[5]);
            close_data.push_back(close);

        }

    }
    Sygnal ret;
    ret.state = SignalStates::HOLD;
    if (!close_data.empty()){
        for (const auto& val:close_data){
            average +=(float)val;
        }
        average /= close_data.size();

        if (close_data.back() > average){
            ret.state = SignalStates::BUY;
        }
    }
    return ret;
}