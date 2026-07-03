#include <iostream>
#include <atomic>
#include <memory>
#include <string>
using namespace std;

class PriceBuffer{

    public:
        void write_to_buffer(string in);
        string get_buffer();
        string exchange_buffer(string in);
        

    private:
        atomic <shared_ptr<const string>> buffer;

};