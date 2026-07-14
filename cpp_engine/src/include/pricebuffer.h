#ifndef PRICE_BUFFER_H
#define PRICE_BUFFER_H
#include <string>
#include <deque>
#include <vector>
#include <mutex>
using namespace std;

class PriceBuffer{
public:
    static constexpr size_t CAPACITY = 1024;

    PriceBuffer() = default;

    void write_to_buffer(string in);
    vector<string> last_n(size_t n);

private:
    deque<string> buffer_;
    mutex m_;
};
#endif