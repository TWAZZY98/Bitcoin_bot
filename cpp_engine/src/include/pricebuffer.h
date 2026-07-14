#ifndef PRICE_BUFFER_H
#define PRICE_BUFFER_H
#include <iostream>
#include <atomic>
#include <memory>
#include <string>
using namespace std;

class PriceBuffer{

    public:
        PriceBuffer(string in);
        PriceBuffer();
        void write_to_buffer(string in);
        string get_buffer();
        string exchange_buffer(string in);
        

    private:
        atomic <shared_ptr<const string>> buffer;

};
#endif