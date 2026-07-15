#ifndef PRICE_BUFFER_H
#define PRICE_BUFFER_H
#include <string>
#include <deque>
#include <vector>
#include <mutex>
#include <atomic>
using namespace std;

class PriceBuffer{
public:
    static constexpr size_t CAPACITY = 1024;

    struct Snapshot{
        uint64_t id;
        deque<string> data;
    };

    PriceBuffer() = default;

    void write_to_buffer(string in);
    deque<string> last_n(size_t n) const;
    bool operator==(const PriceBuffer &o) const;
    PriceBuffer& operator=(const PriceBuffer &o);
    uint64_t getId() const;
    Snapshot getSnapshot()const;
    Snapshot getSnapshot(int n)const;

private:
    atomic<uint64_t> id_{0};
    deque<string> buffer_;
    mutable mutex m_;
};
#endif